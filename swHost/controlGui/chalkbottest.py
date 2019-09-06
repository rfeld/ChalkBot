import sys
import chalkbot

try:

    myBot = chalkbot.chalkBot()

    myBot.move(100, 'b', 1)
    myBot.move(20)

    myBot.turn(45,"ccw", 1)
    myBot.turn(30)

    myBot.stop()

    myBot.chalk("up")
    myBot.chalk("down")

    myBot.stepper("en")
    myBot.stepper("dis")
    try:
        myBot.stepper("quatsch")
    except:
        print("OK")
    else:
        raise Exception("Failed to detect wrong argument")

except:
    print("FAIL!")
    raise

print("PASS")