#Importerer moduler som vi trenger:
import requests                  #for å sende forespørsel om data fra eksterne kilder
from datetime import datetime    #for å time hendelser
import time                      #for å bruke den
import json                      #for å behandle data, vise dem i et mer brukervennlig format

#Lagrer min personlig token og keys til ulike signaler på Circus of Things

token ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MjM1In0.p0385WG1WdjnypiJ0vfSyO0QRs7nVICkRrpdVCDsnFs'
tokenR ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTcyIn0.gOJ7S_SUuPtuoX4Y84oeSiotdWPTdrn4T1zcjynPpKk'
token = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0'
token_Are = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI2MDQwIn0.kta-kE9IMyj9agxtGUrIZ2eFZ_25HpzKNkBpmxSdp5s'

stove_key = '17548'
oven_key = '1146'
bathroom_key = '16655'

daily_consum_key = '6877'
weekly_consum_key = '30188'
monthly_consum_key = '22800'

#Initierer startverdier for strømforbruket på de ulike enhetene
oven_start_value = 0
stove_start_value = 0
bathroom_start_value = 0

#Initierer lister som skal logge forbruket over lengre perioder
weekly_consumption = []
monthly_consumption = []
consumption_so_far = 40000  
#consumption_so_far er en fast startverdi for strømmen som vi forbruker uansett, f.eks. kjøleskap osv.

while True:
    
    #Henter info om baderomstiden som ble valgt i bookingsystemet
    bathroom = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_key, 'Token':tokenR})
    #Lagrer info i python-vennlig format og ekstrakter "Value" som vi er mest interessert i 
    bathroom = bathroom.json()['Value']
    
    #Henter info om kokeplatetiden som ble valgt i bookingsystemet
    stove = requests.get('https://circusofthings.com/ReadValue', {'Key':stove_key, 'Token':tokenR})
    #Lagrer info i python-vennlig format og ekstrakter "Value" som vi er mest interessert i 
    stove = stove.json()['Value']
    
    #Henter info om komfyrtiden som ble valgt i bookingsystemet
    oven = requests.get('https://circusofthings.com/ReadValue', {'Key':oven_key, 'Token':tokenR})
    #Lagrer info i python-vennlig format og ekstrakter "Value" som vi er mest interessert i 
    oven = oven.json()['Value']
    
    
    #    Dersom badet er booket og det var ikke booket forrige gang vi sjekket
    if ((bathroom > 0) and (bathroom != bathroom_start_value)) : 
        #da beregner vi forbrukt effekt ved å gange reservasjonstiden 
        #med den faste verdien som baderommet forbruker.
        consumption_so_far += 0.5 * (bathroom/60)  
        
        #Deretter lagrer vi reservasjonstiden som den forrige avleste verdien.
        #Dette gjør vi for å kunne sammenligne verdiene neste gang vi sjekker
        bathroom_start_value = bathroom
        
    #Gjentar prosedyren med kokeplaten og ovnen 
    if ((stove > 0) and (stove != stove_start_value)) :
        consumption_so_far += 3 * (stove/60)
        stove_start_value = stove
    if ((oven > 0) and (oven != oven_start_value)) :
        consumption_so_far += 3 * (oven/60)
        oven_start_value = oven
    
    
    #Lager en dictionary med data som skal sendes til Circus of Things og sender et signal
    daily_consumption_data = {'Key':daily_consum_key,'Token':token_Are,'Value':consumption_so_far}
    requests.put('https://circusofthings.com/WriteValue', data=json.dumps(daily_consumption_data))
    
    #Lagrer tidspunktet for dagens midnatt 
    today_midnight = datetime.now() 
    today_midnight = today_midnight.replace(hour=0, minute=0, second=0, microsecond=0)
    #Lagrer klokeslettet akkurat nå som et variabel
    now = datetime.now()
    
    if now == today_midnight:  #Dersom det er midnatt nå
        weekly_consumption.append(consumption_so_far)  #da legger vi til dagens forbruk inn i ukeregisteret 
        monthly_consumption.append(consumption_so_far) #og i månedsregisteret
        consumption_so_far = 40000             #restarter strømmåleren 
        
        
    if len(weekly_consumption) <= 7:    #Dersom antall registrerte verdier er færre enn 7 dager
        weekly_consum = sum(weekly_consumption)          #da summerer vi verdiene i ukeregisteret
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal
        weekly_consum_data = {'Key':weekly_consum_key,'Token':token_Are,'Value':weekly_consum}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(weekly_consum_data))
    else:
        weekly_consumption = []  #Ellers restarter vi det ukentlige regnskapet
    
    if len(monthly_consumption) <= 30:        #Dersom antallet registrerte verdier er færre enn 30 dager
        monthly_consum = sum(monthly_consumption)     #Da summerer vi verdiene i månedsregisteret
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal 
        monthly_consum_data = {'Key':monthly_consum_key,'Token':token_Are,'Value':monthly_consum}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(monthly_consum_data))
    else:
        monthly_consumption = []   #Ellers restarter vi det månedlige regnskapet
    
    
    #Venter fem minutter før vi utfører alt på nytt
    time.sleep(300)
    
    

