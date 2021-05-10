#Importerer nødvendige moduler:
import time      #tid
import requests    #henting av eksterne data
import json        #behandling av data slik at det blir mer python-vennlig

#Lagrer min personlig token og keys til baderommet på Circus of Things
token ='eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTcyIn0.gOJ7S_SUuPtuoX4Y84oeSiotdWPTdrn4T1zcjynPpKk'
bathroom_key = "16655"
bathroom_q_key1 = "15932"
bathroom_q_key2 = "10031"
bathroom_q_key3 = "22177"
bathroom_q_key4 = "21336"


while True:
    #Henter info om tiden som ble valgt i bookingsystemet
    bathroom = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_key, 'Token':token})
    #Lagrer info i python-vennlig format og ekstrakter "Value" som vi er mest interessert i 
    bathroom = bathroom.json()['Value']
    
    
    if bathroom != 0:     #dersom noen har booket fasiliteten 
        bathroom = bathroom*60     #multipliserer antall valgte minutter med 60 sekunder
        time.sleep(bathroom)     #og teller ned det tilsvarende antallet sekunder.
        bathroom = 0             #Når nedtellingen er ferdig endrer vi gjenstående tiden til 0
    
    #Lagrer verdier på køplass nummer 1 i CoT som et variabel
    bathroom_queue1 = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_q_key1, 'Token':token})
    bathroom_queue1 = bathroom_queue1.json()['Value']
    
    #Lagrer verdier på køplass nummer 2 i CoT som et variabel
    bathroom_queue2 = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_q_key2, 'Token':token})
    bathroom_queue2 = bathroom_queue2.json()['Value']
    
    #Lagrer verdier på køplass nummer 3 i CoT som et variabel
    bathroom_queue3 = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_q_key3, 'Token':token})
    bathroom_queue3 = bathroom_queue3.json()['Value']
    
    #Lagrer verdier på køplass nummer 4 i CoT som et variabel
    bathroom_queue4 = requests.get('https://circusofthings.com/ReadValue', {'Key':bathroom_q_key4, 'Token':token})
    bathroom_queue4 = bathroom_queue4.json()['Value']
    
    if bathroom_queue1 != 0:      #Dersom noen har skrevet seg oppå ventelisten til badet
        bathroom = bathroom_queue1     #Så er den valgte tiden på køplass nummer 1 den nye reservasjonstiden 
        bathroom_queue1 = bathroom_queue2     #Køplass 2 blir flyttet opp til køplass 1
        bathroom_queue2 = bathroom_queue3     #Køplass 3 blir flyttet opp til køplass 2
        bathroom_queue3 = bathroom_queue4     #Køplass 4 blir flyttet opp til køplass 3
        bathroom_queue4 = 0

        
        #og sender informasjonen om oppdaterte køplasser til Circus of Things
        #  1)Baderommet
    bathroom_data = {'Key':bathroom_key,'Token':token,'Value':bathroom} 
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(bathroom_data))
    #   2) Køplass nummer 1
    bathroom_q1_data = {'Key':bathroom_q_key1,'Token':token,'Value':bathroom_queue1} 
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(bathroom_q1_data))
    #     3)Køplass nummer 2
    bathroom_q2_data = {'Key':bathroom_q_key2,'Token':token,'Value':bathroom_queue1} 
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(bathroom_q1_data))
    #     4) Køplass nummer 3
    bathroom_q3_data = {'Key':bathroom_q_key3,'Token':token,'Value':bathroom_queue1} 
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(bathroom_q1_data))
    #     5) Køplass nummer 4
    bathroom_q4_data = {'Key':bathroom_q_key4,'Token':token,'Value':bathroom_queue4} 
    signal = requests.put('https://circusofthings.com/WriteValue', data=json.dumps(bathroom_q4_data))
     
     

