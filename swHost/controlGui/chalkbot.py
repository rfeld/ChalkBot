class chalkBot:
    def __init__(self):
        self.__doc__="Generates Commands from API Calls"

    # Actually just return command. Offers possible place to hook debug messages etc.
    def returnCommand(self, command = str()):
        #print( command)
        return command

    # Creates command string from command and parameters
    def createCommandString(self, cmdString, parameters=[]):
        return cmdString+":"+",".join(parameters)

    def move(self, distance):
        return self.returnCommand(self.createCommandString("move", [str(distance)]))

    def turn(self, angle):
        return self.returnCommand(self.createCommandString("turn", [str(angle)]))

    def circle(self, angle, radius):
        return self.returnCommand(self.createCommandString("circle", [str(angle), str(radius)]))

    def stop(self):
        return self.returnCommand(self.createCommandString("stop", []))

    def chalk(self, action):
        return self.returnCommand(self.createCommandString("chalk", [str(action)]))

    def stepper(self, status = "dis"):
        if status != "dis" and status != "en":
            raise Exception("Wrong argument status to chalkBot.stepper()")
        return self.returnCommand(self.createCommandString("stepper", [str(status)]))

    
