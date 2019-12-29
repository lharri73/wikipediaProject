from MySQLdb import _mysql
import csv
import re

def main():

    mydb = _mysql.connect(
        host="34.68.183.57",
        user="root",
        passwd="exwmaCrxx8zvPCCl",
        db="results"
    )
    mydb.query("SELECT COUNT(*) FROM positive")
    num = mydb.store_result()
    print(num.fetch_row())

    with open("csvFiles/Hitler.csv", "r") as csvFile:
        reader = csv.reader(csvFile)
        for row in reader:
            # print(row[0])
            if row[0] == "NOT POSSIBLE":
                not_possible(mydb, row[1])
            else:
                positive(mydb, row)
    
    mydb.query("SELECT COUNT(*) FROM positive")
    num = mydb.store_result()
    print(num.fetch_row())


def not_possible(mydb, page_name):
    assert type(page_name) == str, "page_name must be a string"
    mydb.query("INSERT INTO negative(name) values (\"{}\")".format(escape_special(page_name)))

def positive(mydb, row):
    mydb.query("INSERT INTO positive(first, second, third, fourth) VALUES (\"{}\", \"{}\", \"{}\", \"{}\")".format(escape_special(row[0]), escape_special(row[1]), escape_special(row[2]), escape_special(row[3])))

def escape_special(name):
    escaped = name.translate(str.maketrans({
                                        "\'":  r"\'",
                                        "\\": r"\\",
                                        "\"":  r"\"",
                                        "?":  r"\?",}))
    return escaped

if __name__ == "__main__":
    main()


