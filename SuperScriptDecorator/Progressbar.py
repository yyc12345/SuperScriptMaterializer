import sys

class Prograssbar(object):
    def __init__(self, filecount: int):
        if (filecount < 0): raise Exception("Progressbar can not hold minus length!")

        self.__FileCount: int = filecount
        self.__FileNow: int = 0
        self.__ContentCount: int = 0
        self.__ContentNow: int = 0

        self.__PbarFullChar: int = 50
        self.__PercentPerFile: float = 1 / self.__FileCount
        self.__CurFileName: str = None

        self.__Render()

    def StepFile(self, newfile: str, content_len: int):
        if self.__CurFileName is not None:
            # not first call, INC FileNow
            # if first call, do not INC it
            self.__FileNow += 1
        # apply others
        self.__CurFileName = newfile
        self.__ContentNow = 0
        self.__ContentCount = content_len

        self.__Render()

    def StepContent(self):
        self.__ContentNow += 1

        self.__Render()

    def Finish(self):
        sys.stdout.write('\n')
        sys.stdout.flush()

    def __Render(self):
        percentage_content: float = 0 if self.__ContentCount == 0 else (self.__ContentNow / self.__ContentCount)
        percentage_full: float = (percentage_content + self.__FileNow) * self.__PercentPerFile

        percentage_bar = int(percentage_full * self.__PbarFullChar)

        sys.stdout.write('\r[{}{}] {:.2f}% - {}'.format(
            percentage_bar * '#',
            (self.__PbarFullChar - percentage_bar) * '=', 
            percentage_full * 100,
            self.__CurFileName if self.__CurFileName else ''
        ))
        sys.stdout.flush()
