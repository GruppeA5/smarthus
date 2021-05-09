#Importerer modulene vi trenger:
from entsoe import EntsoePandasClient     #for å få tilgang til strømpriser
import pandas as pd                      #for å behandle og analysere data
import requests                         #for å sende forespørsel om data til eksterne kilder
import json                         #for å behandle data, vise dem i et mer brukervennlig format
import time                             #for å kødde med tid

#Lagrer kommandoen "hent data vha min API" 
client = EntsoePandasClient(api_key='61402fbb-cc17-4eaa-9422-7c3508d08601')

#Lagrer våre parametre i ENTSOE Transparency Platform
start = pd.Timestamp(tz='Europe/Brussels', year=2021, month=4, day=23, hour=0, minute=0, second=0)
end = pd.Timestamp.now(tz='Europe/Brussels')
country_code = '10YNO-1--------2'  # Norges kode 

#Lagrer keys til ulike signaler på CoT 
key_elexpense_week = "23616";
key_elexpense_month = "25052";
key_elexpense_year = "18360";
el_consum_hourly_key = '6877'
solar_key = '17378'
electricity_price_key = '31254'

#Lagrer tokens på CoT til alle involverte brukere
token ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MjM1In0.p0385WG1WdjnypiJ0vfSyO0QRs7nVICkRrpdVCDsnFs'
token_Jul ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0'
token_mat = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1OTAwIn0.ieCdl0qd5EgIahhOKKSkhOOaSYICXKHcJYfjduC2vCo'
token_Are = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI2MDQwIn0.kta-kE9IMyj9agxtGUrIZ2eFZ_25HpzKNkBpmxSdp5s'

#Initierer lister som skal logge verdier på strømregningen over lengre perioder 
previous_el_use = 0
el_bill_hourly = 0
el_bill_weekly = []
el_bill_monthly = []
el_bill_yearly = []


while True: 
    # Henter strømprisen fra ENTSOE
    prices = client.query_day_ahead_prices(country_code, start=start,end=end)
    #Prisen nå er da den siste og nyligste verdien i prices - series
    #Bruker indeksering for series og lagrer prisen nå som et variabel
    electricity_price_now = prices[-1]
    
    #Henter valutakurset fra Norges Bankens databaser og endrer format til json
    currency_exchange = requests.get("https://data.norges-bank.no/api/data/EXR/B.EUR.NOK.SP?lastNObservations=1&format=sdmx-json&locale=no")
    currency_exchange = currency_exchange.json()
    
    #Henter ut valutakurset fra innhentede data
    currency_exchange = currency_exchange['data']['dataSets'][0]['series']['0:0:0:0']['observations']['0'][0]
    #Endrer datatypen fra string til float 
    currency_exchange = float(currency_exchange)
    
    #Beregner den endelige strømprisen i NOK 
    electricity_price_NOK = electricity_price_now * currency_exchange
        
    #Sender signalet med info om strømprisen til smartmeteret
    electricity_data={'Key':electricity_price_key,'Token':token,'Value':electricity_price_NOK} 
    signal_electricity = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(electricity_data))
    
    #Sender forespørsel om tilgang til informasjon om effektforbruket og lagrer verdien 
    el_use_hourly = requests.get('https://circusofthings.com/ReadValue', {'Key':el_consum_hourly_key, 'Token':token_Are})
    el_use_hourly = json.loads(el_use_hourly.content)['Value']
    
    #Sender forespørsel om tilgang til informasjon om produsert solenergi og lagrer verdien
    solar_energy = requests.get('https://circusofthings.com/ReadValue', {'Key':solar_key, 'Token':token_Jul})
    solar_energy = json.loads(solar_energy.content)['Value']
    
    #Finner forbruket ila den siste timen ved å trekke den forrige avleste verdien fra den nyligste verdien
    el_use_hour = el_use_hourly - previous_el_use
    
    #Lagrer det nylig avleste forbruket som den forrige avleste verdien 
    #Dette for å kunne bruke det neste gang koden kjører, altså om en time
    previous_el_use = el_use_hourly
    
    #Beregner prisen for den strømmen vi har forbrukt ila den siste timen
    #I formelen tar vi hensyn til solenergien vi produserte
    el_bill_hourly = (el_use_hour - solar_energy) * electricity_price_NOK
    
    #Strømregningen ila siste timen legges til i:
    el_bill_weekly.append(el_bill_hourly)     #strømregningen for siste uken
    el_bill_monthly.append(el_bill_hourly)    #strømregningen for siste måned
    el_bill_yearly.append(el_bill_hourly)     #strømregningen for siste år
    
    
    if len(el_bill_weekly) <= 168:   #Dersom antall registrerte verdier i ukeregningen er mindre enn 7 dager
        el_bill_week = sum(el_bill_weekly)          #da summerer vi verdiene 
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal
        el_bill_weekly_data = {'Key':key_elexpense_week,'Token':token_mat,'Value':el_bill_week}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(el_bill_weekly_data))
    else:
        el_bill_weekly = []  #Ellers restarter vi det ukentlige regnskapet
    
    
    
    #Gjentar prosedyren for den månedlige regningen:

    if len(el_bill_monthly) <= 720:    #Dersom antall registrerte verdier er mindre enn 30 dager
        el_bill_month = sum(el_bill_monthly)          #da summerer vi verdiene 
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal
        el_bill_monthly_data = {'Key':key_elexpense_month,'Token':token_mat,'Value':el_bill_month}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(el_bill_monthly_data))
    else:
        el_bill_monthly = []  #Ellers restarter vi det månedlige regnskapet
    
    
    
    #Gjentar prosedyren for den årlige regningen:
    
    if len(el_bill_yearly) <= 8760:    #Dersom antall registrerte verdier er mindre enn 365 dager
        el_bill_year = sum(el_bill_yearly)          #da summerer vi verdiene 
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal
        el_bill_yearly_data = {'Key':key_elexpense_year,'Token':token_mat,'Value':el_bill_year}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(el_bill_yearly_data))
    else:
        el_bill_yearly = []  #Ellers restarter vi det årlige regnskapet
        
    #Venter en time før vi gjentar hele prosedyren
    time.sleep(3600)

