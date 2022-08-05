import locale

class DatabaseType:
    SQLite = 0
    MySQL = 1

database_type = DatabaseType.SQLite
sqlite_path = "decorated.db"

'''
database_type = DatabaseType.MySQL
mysql_url = "http://yyc.bkt.moe:10000"
mysql_username = "test"
mysql_password = "test"
mysql_database = "test_database"
'''

debug_mode = False
