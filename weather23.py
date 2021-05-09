# Importerer moduler
import requests, json
from sense_hat import SenseHat
sense = SenseHat()

# Lagrer min personlige API-keyen for openweathermap.com
api_key = "0786557cdda7fa1340e3887e5d5fe3c2"

#Lagrer min CoT-informasjon
token ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0'
humidity_key = '25756'
temp_key = '1536'
wind_key = '22794'

# Lagrer grunnlaget til URL-adressen som en variabel
base_url = "https://api.openweathermap.org/data/2.5/onecall?"

# Oppgir koordinater til en værstasjon i Trondheim
lat='63.4305'
lon='10.3951'

# Skriver ferdig URL-adresse med alle variabler
complete_url = base_url + "lat=" + lat + "&lon=" + lon + "&appid=" + api_key

while True:
    # Definerer response som svaret på forespørsel om værdata fra ovennevnte URLen
    response = requests.get(complete_url)
    
    # Konverterer responsen til et python-vennlig dictionary-format
    x = response.json()
    
    # Henter temperatur og vindstyrke fra vår dictionary
    y = x["current"]
    message1 = y["temp"]
    message2 = y["wind_speed"]
    message3 = y["humidity"]
    #Printer melding
    sense.show_message("Temperatur: " + message1 + "ºC; Vindstyrke: " + message2 + "km/h; Luftfuktighet: " + message3 + "% Ha en fin dag!")
    
    #Lagrer dictionaries som inneholder data som skal sendes til Circus of Things
    temp_data={'Key':temp_key,'Token':token,'Value':message1} 
    humidity_data={'Key':humidity_key,'Token':token,'Value':message3} 
    wind_data={'Key':wind_key,'Token':token,'Value':message2} 
    
    #Sender tre signaler til Circus of Things
    cot_temp = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(temp_data))
    cot_humidity = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(humidity_data))
    cot_wind = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(wind_data))


