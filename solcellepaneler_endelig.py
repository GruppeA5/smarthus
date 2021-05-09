# import required modules
import requests, json
import time

#Lagrer min personlig token og keys til solenergi-panelet på Circus of Things
solar_key = '17378'
daily_solar_key = '11674'
weekly_solar_key = '32218'
monthly_solar_key = '11494'

token ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0'
token_Are = 'eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI2MDQwIn0.kta-kE9IMyj9agxtGUrIZ2eFZ_25HpzKNkBpmxSdp5s'

# Definerer personlig API-token for openweathermap.com
api_key = '0786557cdda7fa1340e3887e5d5fe3c2'
  
# Definerer grunnlaget for å komponere URL som inneholder værdata
base_url = "https://api.openweathermap.org/data/2.5/onecall?"

# Definerer Trondheims kartkoordinater 
lat='63.4305'
lon='10.3951'

# Definerer den ferdige url adressen
complete_url = base_url + "lat=" + lat + "&lon=" + lon + "&appid=" + api_key
  
#Initierer en liste som skal logge produsert solenergi
solar_power = []
daily_solar_power = []
weekly_solar_power = []
monthly_solar_power = []

while True:
    # Definerer weather som svaret på forespørsel om værdata fra ovennevnte URLen
    weather = requests.get(complete_url)
    
    # Bruker json modulen til å gjøre weather om til python-vennlig dictionary 
    weather = weather.json()
  
    # Vi er kun interessert i nåværende vær. 
    # Lagrer det i variabelen y
    y = weather["current"]
    
    # Henter og lagrer nåværende skydekke og UV-indeksen som variabler  
    current_clouds = y["clouds"]
    uv_index = y["uvi"]
    
    if uv_index != 0:   #hvis solen er oppe (produserer UV-stråling)
        # Beregner produsert solenergi vha. en formel som vi selv formulerte  
        solenergi = (320 *(1-current_clouds/100) * 0.84) * 150
    else: solenergi = 0    #ellers produserer ikke solceller energi
    
    #Definerer en dictionary med info til Circus of Things
    sol_data={'Key':solar_key,'Token':token,'Value':solenergi} 
     
    #Sender et signal til Circus of Things
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(sol_data))
    
    daily_solar_power.append(solenergi)    #Hver beregnet verdi legges inn i et daglig register
    
    if len(daily_solar_power) <= 48:     #Dersom antall registrerte verdier i listen overstiger 24 timer: 
        day_solar_power = sum(daily_solar_power)  #da summerer vi registrerte verdier
        
        #Lager en dictionary med data som skal sendes til Circus of Things og sender et signal
        daily_sol_data = {'Key':daily_solar_key,'Token':token_Are,'Value':day_solar_power}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(daily_sol_data))
    else:
        daily_solar_power = []    #Ellers restarter det daglige regnskapet
        
        
    #Gjør det samme for det ukentlige registeret
    
    weekly_solar_power.append(solenergi)  #Hver beregnet verdi for solenergi legges til i det ukentlige registeret
    if len(weekly_solar_power) <= 336:    #Dersom antall registrerte verdier overstiger 7 dager
        week_solar_power = sum(weekly_solar_power)          #da summerer vi verdiene 
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal
        weekly_sol_data = {'Key':weekly_solar_key,'Token':token_Are,'Value':week_solar_power}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(weekly_sol_data))
    else:
        weekly_solar_power = []  #Så restarter vi det ukentlige regnskapet
    
    
    
    #Gjentar prosedyren for det månedlige registeret
    
    monthly_solar_power.append(solenergi) #Hver beregnet verdi for solenergi legges til i det månedlige registeret
    
    if len(monthly_solar_power) <= 1440:        #Dersom antallet registrerte verdier er mindre enn 30 dager
        month_solar_power = sum(monthly_solar_power)     #Da summerer vi verdiene 
        
        #Og lager en dictionary med data som sendes til CoT og sender et signal 
        monthly_sol_data = {'Key':monthly_solar_key,'Token':token_Are,'Value':month_solar_power}
        requests.put('https://circusofthings.com/WriteValue', data=json.dumps(monthly_sol_data))
    else:
        monthly_solar_power = []   #Ellers restarter vi det månedlige regnskapet
          
        
    #Venter en halvtime før vi utfører alt på nytt
    time.sleep(1800)
    
    

    
