import locale, sys, os, shlex

class InputEntry(object):
    def __init__(self, data: tuple[str]):
        if len(data) != 4:
            raise Exception(f"Input syntax error. Require 4 items but got {len(data)}")
        self.m_Name: str = data[0]
        self.m_VtFile: str = data[1]
        self.m_ExportDb: str = data[2]
        self.m_EnvDb: str = data[3]

class CustomConfig(object):
    def __init__(self):
        # encoding list
        # https://docs.python.org/3/library/codecs.html#standard-encodings
        self.m_DatabaseEncoding: str = locale.getpreferredencoding()
        self.m_DebugMode = False

        self.m_ImportTxt: str = None
        self.m_DecoratedDb: str = None

        self.m_InputEntries: list[InputEntry] = []

    def Regulate(self) -> bool:
        # check input and output
        if self.m_ImportTxt is None:
            print("No input. Decorator exit.")
            return False
        if not os.path.isfile(self.m_ImportTxt):
            print(f'No such input: "{self.m_ImportTxt}"')
            return False

        if self.m_DecoratedDb is None:
            print("No output. Decorator exit.")
            return False
        if os.path.isdir(self.m_DecoratedDb):
            print("Output must be a file.")
            return False

        # remove result database in debug mode
        if self.m_DebugMode and os.path.isfile(self.m_DecoratedDb):
            os.remove(self.m_DecoratedDb)

        # process input file
        try:
            with open(self.m_ImportTxt, 'r', encoding='utf-8') as f:
                while True:
                    ln = f.readline()
                    if ln == '': break
                    ln.strip()
                    if ln == '': continue

                    self.m_InputEntries.append(InputEntry(shlex.split(ln)))
        except Exception as ex:
            print("Errro when processing input file.")
            print(ex)

        return True
