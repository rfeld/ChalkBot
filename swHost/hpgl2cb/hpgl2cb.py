# hpgl2cb - read HPGL file and translate it to Chalkbot Comands
# 07. Sept 2019,  Robert Feld
# -------------------------------------------------------------
import sys
# Add path to reusable cb code
sys.path.append('../controlGui')
import chalkbot
import hpgl

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

    # Here is the action triggered
    with open(hpgl_file_name,'r') as hpfile:
        output = parse_hpgl(hpfile)
        hpglqueue = read_hpgl_to_queue( hpfile )
        cbqueue = convert_hpgl_to_cb( hpglqueue )
        print(cbqueue)

# Read hpgl file and store each command with its parameters as a list in a list
def read_hpgl_to_queue( hpfile ):
    return ["PU", 0,0,10,10] # ToDo: Implement

def convert_hpgl_to_cb( hpglqueue ):
    return ["turn", 3600] # ToDo: Implement

if __name__ == "__main__":
    main()