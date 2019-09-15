# hpgl2cb - read HPGL file and translate it to Chalkbot Comands
# 07. Sept 2019,  Robert Feld
# -------------------------------------------------------------
import sys
# Add path to reusable cb code
sys.path.append('../controlGui')
import chalkbot
from vectorstuff.vector import vector
from vectorstuff.state_vector import state_vector

cb = chalkbot.chalkBot()

def main():
    # Read file to be opened from command line
    # Expected Format: hpgl2cb.py <filename.hpgl>
    if len(sys.argv) != 2: 
        print("usage: " + sys.argv[0] + " <filename.hpgl>")
        exit(1)
    else:
        hpgl_file_name = sys.argv[1]
    
    print("Trying to open "+hpgl_file_name)

    # Here is where the action is...
    with open(hpgl_file_name,'r') as hpfile:
        hpglqueue = read_hpgl_to_queue( hpfile )
        cbqueue = convert_hpgl_to_cb( hpglqueue )
        print(cbqueue)

# Read hpgl file and store each command with its parameters as a list in a list
def read_hpgl_to_queue( hpfile ):
    
    # List to be filled and returned
    retQ = list()

    for line in hpfile:
        line = line.rstrip()
        commands = line.split(';')
        for command in commands:
            if command != "":
                #print("Command: " + command[:2] + " Parameter: " + command[2:])
                two_letter_command = [command[:2]]
                parameter_sub_list = command[2:].split(',')
                parameter_sub_list_int = list()
                for parameter in parameter_sub_list:
                    # Scale Coordinates to mm for PU and PD commands
                    if two_letter_command[0] == "PU" or two_letter_command[0] == "PD":
                        parameter_sub_list_int.append(int(parameter)*2.5/1000)
                    elif two_letter_command != "" and parameter != "":
                        parameter_sub_list_int.append(int(parameter))
                command_sub_list = two_letter_command + parameter_sub_list_int
                #print( command_sub_list)
                retQ.append(command_sub_list)
    return retQ



def convert_hpgl_to_cb( hpglqueue ):

    # The origin of the global coordinate system will be assigned to the first place, the pen is put down
    origin = vector(0,0)
    cb_state_vector =  state_vector() # where in the global coordinate space is the chalk bot currently
    flag_drawing_started = False # Remember, whether drawing (PD) has already started

    for command in hpglqueue:
        if command[0] == "IN":
            print(" - Init command is skipped")
        elif command[0] == "SP":
            print(" - Pen selection command is not used (yet...)")
        elif command[0] == "PU":
            if flag_drawing_started:
                print(" + Movement with pen up")
            else:
                print(" - Movement with pen up before drawing --> ignored")
            # update state vector
            cb_state_vector.x=1
        elif command[0] == "PD":
            if not flag_drawing_started:
                flag_drawing_started = True
                print(" + First time pen is put down --> Setting global origin")
            print(" + Movement with pen down --> Drawing!")
    return ["turn", 3600] # ToDo: Implement

if __name__ == "__main__":
    main()