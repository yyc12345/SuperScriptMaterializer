import CustomConfig
import DecoratorCore
import ServerCore
import os
import sys
import getopt

try:
    opts, args = getopt.getopt(sys.argv, "hi:o:e:f")
except getopt.GetoptError:
    print('Wrong arguments!')
    print('python SuperScriptViewer.py -i <export.db> -o <decorated.db> -e <env.db> -f')
    sys.exit(1)
for opt, arg in opts:
    if opt == '-h':
        print('python SuperScriptViewer.py -i <export.db> -o <decorated.db> -e <env.db> -f')
        sys.exit(0)
    elif opt == '-i':
        CustomConfig.export_db = arg
    elif opt == '-o':
        CustomConfig.decorated_db = arg
    elif opt == '-e':
        CustomConfig.env_db = arg
    elif opt == '-f':
        CustomConfig.force_regenerate = True

# process -f
if (CustomConfig.force_regenerate):
    if os.path.isfile(CustomConfig.decorated_db):
        os.remove(CustomConfig.decorated_db)

print('Super Script View')
if not os.path.isfile(CustomConfig.env_db):
    print('No environment database. Fail to generate. Exit app.')
    sys.exit(1)

if not os.path.isfile(CustomConfig.decorated_db):
    print('No decorated database, generating it.')
    if not os.path.isfile(CustomConfig.export_db):
        print('No export.db. Fail to generate. Exit app.')
        sys.exit(1)

    # generate db
    DecoratorCore.run()
    print('Decorated database generating done.')

# todo: start flask
ServerCore.run()
