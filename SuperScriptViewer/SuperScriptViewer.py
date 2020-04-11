import DecoratorCore
import ServerCore
import os
import sys

# debug use
os.remove('decorate.db')

print('Super Script View')
if not os.path.isfile("decorate.db"):
    print('No decorated database, generating it.')
    if not os.path.isfile('export.db'):
        print('No export.db. Fail to generate. Exit app.')
        sys.exit(1)

    # generate db
    DecoratorCore.run()
    print('Decorated database generating done.')

# todo: start flask
# ServerCore.run()
