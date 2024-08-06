
class DeclPair():
    m_DeclType: str
    m_DeclName: str

    def __init__(self, decl_type: str, decl_name: str):
        self.m_DeclType = decl_type
        self.m_DeclName = decl_name

    def is_string(self):
        return self.m_DeclType == 'YYCC::yycc_u8string'
    
    def is_int64(self):
        return self.m_DeclType == 'int64_t'

def accept_input() -> tuple[DeclPair, ...]:
    cache: list[DeclPair] = list()
    blank_line: bool = False

    while True:
        recv: str = input("> ")
        if len(recv) == 0:
            # If double blank line, exit
            # Otherwise set blank line flag
            if blank_line: break
            else: blank_line = True
        else:
            # Reset blank line flag
            blank_line = False
            # Analyze input
            recv = recv.strip('\t\r\n ')
            # Skip annotation
            if recv.startswith('//'): continue
            # Skip invalid decl
            if not recv.endswith(';'): continue
            recv = recv[:-1]
            recv_parts = recv.split(' ')
            if len(recv_parts) != 2: continue
            # Okey, insert it
            cache.append(DeclPair(recv_parts[0], recv_parts[1]))

    return tuple(cache)

def generate_result(decls: tuple[DeclPair, ...]) -> tuple[str, str, str]:
    # generate sql statement
    def conv_sql(decl_pair: DeclPair) -> str:
        if decl_pair.is_string(): return f'[{decl_pair.m_DeclName}] TEXT'
        else: return f'[{decl_pair.m_DeclName}] INTEGER'
    table_string: str = ', '.join(map(conv_sql, decls))
    gen_sql_create_statement: str = f'CREATE TABLE [] ({table_string});'

    # generate sql insert_statement
    table_string = ', '.join(map(lambda _: '?', decls))
    gen_sql_insert_statement: str = f'INSERT INTO [] VALUES ({table_string});'

    # generate binding c++ statement
    def conv_cpp(decl_pair: DeclPair) -> str:
        if decl_pair.is_string():
            return f'WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.{decl_pair.m_DeclName}), -1, SQLITE_TRANSIENT));'
        elif decl_pair.is_int64():
            return f'WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.{decl_pair.m_DeclName}));'
        else:
            return f'WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.{decl_pair.m_DeclName}));'
    gen_bind_statement: str = '\n'.join(map(conv_cpp, decls))

    return (gen_sql_create_statement, gen_sql_insert_statement, gen_bind_statement)    
    
if __name__ == '__main__':
    while True:
        decls: tuple[DeclPair, ...] = accept_input()
        (sql_create, sql_insert, cpp) = generate_result(decls)
        print('SQL Create Table Statement:')
        print(sql_create)
        print('SQL Insert Statement:')
        print(sql_insert)
        print('C++ Bind Value Statements:')
        print(cpp)
