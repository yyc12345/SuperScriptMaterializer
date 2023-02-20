import CustomConfig, DecoratorCore, Progressbar
import os, sys, getopt, logging, time

# print banner
print('Super Script Decorator')
print('Homepage: https://github.com/yyc12345/SuperScriptMaterializer')
print('Report bug: https://github.com/yyc12345/SuperScriptMaterializer/issues')
print('')

# try get args
try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:e:c:fd")
except getopt.GetoptError:
    print('Wrong arguments!')
    print('python SuperScriptViewer.py -i <import.txt> -o <decorated.db> -c <codec_name> -d')
    sys.exit(1)

# analyze args
cfg: CustomConfig.CustomConfig = CustomConfig.CustomConfig()
for opt, arg in opts:
    if opt == '-h':
        print('python SuperScriptViewer.py -i <import.txt> -o <decorated.db> -c <codec_name> -d')
        sys.exit(0)
    elif opt == '-i':
        cfg.m_ImportTxt = arg
    elif opt == '-o':
        cfg.m_DecoratedDb = arg
    elif opt == '-c':
        cfg.m_DatabaseEncoding = arg
    elif opt == '-d':
        cfg.m_DebugMode = True

# regulate data
if not cfg.Regulate():
    # failed. exit program
    sys.exit(1)

# if in debug mode, run directly
# otherwise, run with a try wrapper.
if cfg.m_DebugMode:
    DecoratorCore.Run(cfg)
else:
    try:
        DecoratorCore.Run(cfg)
    except Exception as ex:
        print("!!! An error occurs. Please report follwoing error output and reproduce file to developer. !!!")
        logging.exception(ex)
        sys.exit(1)
        
print('Decorated database generation done.')

