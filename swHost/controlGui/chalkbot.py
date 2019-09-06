class chalkBot:
    def __init__(self):
        self.__doc__="Generates Commands from API Calls"

    # Actually just return command. Offers possible playe to hook debug messages etc.
    def sendCommand(self, command = str()):
        #print( command)
        return command

    # Creates command string from command and parameters
    def createCommandString(self, cmdString, parameters=[]):
        return cmdString+":"+",".join(parameters)

    def move(self, distance):
        return self.sendCommand(self.createCommandString("move", [str(distance)]))

    def turn(self, angle):
        return self.sendCommand(self.createCommandString("turn", [str(angle)]))

    def stop(self):
        return self.sendCommand(self.createCommandString("stop", []))

    def chalk(self, action):
        return self.sendCommand(self.createCommandString("chalk", [str(action)]))

    def stepper(self, status = "dis"):
        if status != "dis" and status != "en":
            raise Exception("Wrong argument status to chalkBot.stepper()")
        return self.sendCommand(self.createCommandString("stepper", [str(status)]))

    
