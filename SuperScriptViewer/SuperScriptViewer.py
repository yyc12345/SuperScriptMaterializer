import CustomConfig
import DecoratorCore
import ServerCore
import os
import sys
import getopt
import logging

try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:e:c:fd")
except getopt.GetoptError:
    print('Wrong arguments!')
    print('python SuperScriptViewer.py -i <export.db> -o <decorated.db> -e <env.db> -c <codec_name> -f -d')
    sys.exit(1)
for opt, arg in opts:
    if opt == '-h':
        print('python SuperScriptViewer.py -i <export.db> -o <decorated.db> -e <env.db> -c <codec_name> -f -d')
        sys.exit(0)
    elif opt == '-i':
        CustomConfig.export_db = arg
    elif opt == '-o':
        CustomConfig.decorated_db = arg
    elif opt == '-e':
        CustomConfig.env_db = arg
    elif opt == '-c':
        CustomConfig.database_encoding = arg
    elif opt == '-f':
        CustomConfig.force_regenerate = True
    elif opt == '-d':
        CustomConfig.debug_mode = True

print('Super Script View')
print('Homepage: https://github.com/yyc12345/SuperScriptMaterializer')
print('Report bug: https://github.com/yyc12345/SuperScriptMaterializer/issues')
print('')
# process -f
if (CustomConfig.force_regenerate):
    if os.path.isfile(CustomConfig.decorated_db):
        os.remove(CustomConfig.decorated_db)

if not os.path.isfile(CustomConfig.env_db):
    print('No environment database. Fail to generate. Exit app.')
    sys.exit(1)

if not os.path.isfile(CustomConfig.decorated_db):
    print('No decorated database, generating it.')
    if not os.path.isfile(CustomConfig.export_db):
        print('No export.db. Fail to generate. Exit app.')
        sys.exit(1)

    # generate db
    if CustomConfig.debug_mode:
        DecoratorCore.run()
    else:
        try:
            DecoratorCore.run()
        except Exception as ex:
            print("!!! An error occurs. Please report follwoing error output and reproduce file to developer. !!!")
            logging.exception(ex)
            sys.exit(1)

    print('Decorated database generating done.')

# todo: start flask
ServerCore.run()
