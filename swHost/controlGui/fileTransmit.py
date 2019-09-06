# Grober Plan:

""" 
Daten einlesen
    Lies Zeile aus Datei
    Konvertiere in Listeneintrag

Verbindung aufbauen
Wiederhole bis Daten alle gesendet wurden:
    Wandle Listeneintrag in Cmd-String
    Sende String
    Wenn ACK, entferne aus Liste
    Sonst Warte 2s und versuche eneut """

# Datenformate
# Queue:
# Liste mit Eintraegen vom Format: Command, Par1, Par2, Par3, ...

import chalkbot
import socket
import time
import sys

# IP Adress and Port of the "telnet" connection to the chalk bot
ChalkBotAddress = ("192.168.42.47", 23)

# Read file to be opened from command line
# Expected Format: fileTransmit.py <filename.scb>
if len(sys.argv) != 2: 
    print("usage: " + sys.argv[0] + " <filename.scb>")
    exit(1)

fileToOpen = sys.argv[1]

# Do some checks on the filename

print( fileToOpen )

# Funktion list File ein und lege Kommandos in Liste (aus Listen) ab
# Gibt Liste zurueck
def readSimpleFile( fileName ):
    retQ = list()
    with open(fileName,'r') as f:
        for line in f:
            line = line.rstrip()
            # Falls kein Kommentar
            if line[0] != "#":
                # print("Zeile:" + line)
                # print(line.split(';'))
                retQ.append(line.split(';'))
    return retQ

# Wandle Eintrag aus Liste in cmd-String um
def makeCmdString( queueItem, bot ):
    #return "stop\n"
    #print("QI: " + queueItem[0])
    if queueItem[0] == "stop":
        return bot.stop()
    elif queueItem[0] == "turn":
        return bot.turn(queueItem[1])
    elif queueItem[0] == "move":
        return bot.move(queueItem[1])
    elif queueItem[0] == "chalk":
        return bot.chalk(queueItem[1])
    elif queueItem[0] == "stepper":
        return bot.stepper(queueItem[1])
    else:
        raise Exception("Unknown command")

# Nimm Queue und sende mit Flow Control
# queue   - List with commands to be sent
# bot     - chalkbot object to convert command in protocol message
# address - IP (V4/V6) Address tupel of the chalkbot (host, port)
def sendQueue( queue, bot, address):
    try:
        with socket.create_connection(address, 5) as chasock:
        #with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as chasock:

            # Set Timeout to 15s
            chasock.settimeout(15)

            print("Connected")

            # Reset State of Chalkbot
            chasock.sendall( bytes("\n", "utf-8") )
            time.sleep(1)
            #aw = chasock.recv(128)
            

            # insert initial stop command
            queue.insert(0, ['stop'])

            for item in queue:
                ack = False

                #ToDo: Birgit: Nur gültige in die Queue
                #ToDo: Done korrekt behandeln
                cmd = makeCmdString( item, bot ) + "\n"
                
                print("TX-> " + cmd)
                chasock.sendall( bytes(cmd, "utf-8") )

                while ack != True:
                    # Wait for answer
                    # ToDo: Anworten erst weiter verarbeiten, wenn /n empfangen wurde
                    # ToDo: Debug Meldungen ignorieren ("DEBUG: ...")
                    lineComplete = False
                    aw = bytearray()
                    while lineComplete == False:
                        try:
                            c = chasock.recv(1)
                        except socket.timeout as e:
                            err = e.args[0]
                            if err == 'timed out':
                                print('recv timed out, retry later')
                                # lets see what we got ...
                                lineComplete = True
                                continue
                        #print("Zeichen empfangen" + str(c))
                        if c == b'\n':
                            #print("Zeilenende empfangen")
                            lineComplete = True
                        aw.append(c[0])
                    #print("Antwort zusammengesetzt:" + "".join(map(chr,aw)))
                    #aw = chasock.recv(128)

                    if len(aw) == 0:
                        #raise Exception("Connection closed by CB")
                        print("Empty Answer")
                    print("RX<- " + "".join(map(chr,aw)).rstrip())
                    if aw[0:3] == b"ACK":
                        # Platz in RX queue des chalk bot bestimmen
                        freeSlots = int(aw[4:])
                        print("ACK. Platz in RX Queue: " + str(freeSlots))
                        if freeSlots >0:
                            ack = True
                    elif aw[0:7] == b"ERR: 3,":
                         print("Flow Control: Wait!")
                         # add command back in the queue
                         queue.insert(0, item)
                         time.sleep(10)
                    elif aw[0:3] == b"ERR":
                        #raise Exception(str(aw))
                        queue.insert(0, ['stop'])
                        print("ERROR reported -> stop")
                    elif aw[0:4] == b"DONE":
                        print("DONE")
                        ack = True
                    else:
                        #raise Exception("Unknown Error")
                        queue.insert(0, ['stop'])
                        print("Unknown Command  -> stop")
                print("ACK!")

    except Exception as e: 
        print("Error occurred while transmitting Commands to Chalkbot:")
        print(e)
        print("Sending STOP")
        
        raise
    

# Hier wird dann alles zusammengesetzt für einen Test
if __name__ == "__main__":
    myBot = chalkbot.chalkBot()

    
    cmdQueue = readSimpleFile("quadrat.scb")
    print(cmdQueue)
    sendQueue(cmdQueue, myBot, ChalkBotAddress)

    #cmdQueue = readSimpleFile("fullTurn.scb")
    
    # cmdQueue = readSimpleFile("stop.scb")
    # print(cmdQueue)
    # sendQueue(cmdQueue, myBot, ChalkBotAddress)



    
                
            
