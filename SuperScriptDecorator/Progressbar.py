import sys

value_All = 0
value_Now = 0
progressbar_span = 2
progressbar_count = int(100/progressbar_span)

def initProgressbar(all):
    global value_Now, value_All
    value_All = all
    value_Now = 0

    sys.stdout.write('[{}] 0%'.format(progressbar_count * '='))
    sys.stdout.flush()

def stepProgressbar():
    global value_Now, value_All
    value_Now += 1
    if (value_Now > value_All):
        value_Now = value_All

    percentage = int(value_Now / value_All * 100)
    percentage_bar = int(value_Now / value_All * progressbar_count)
    sys.stdout.write('\r[{}{}] {}%'.format(percentage_bar * '#',(progressbar_count - percentage_bar) * '=', percentage))
    sys.stdout.flush()

def finProgressbar():
    sys.stdout.write('\r[{}] 100%\n'.format(progressbar_count * '#'))
    sys.stdout.flush()