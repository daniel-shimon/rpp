from tests.utils import Test, main

code = '''
מחלקה איש:
    סהכ = 0
    פעולה __אתחל__(מספר):
        אני.מספר = מספר
        אני.סהכ = אני.סהכ + 1
    חישוב = פעולה(מ) החזר מ + אני.מספר

פלוט איש(1).חישוב(10) שווהל 11
איש(8)
פלוט איש.סהכ == 2

פעולה מפעל(מ):
    החזר מחלקה:
        סטאטי = מ
        פעולה __אתחל__() אני.סטאטי = אני.סטאטי + 1

מ1 = מפעל(1)
פלוט מ1.סטאטי == 1
מ1()
פלוט מ1.סטאטי == 2

מ2 = מפעל(100)
''' + '\n'.join('מ2()' for _ in range(200)) + '''
פלוט מ2.סטאטי
'''

expected = 'amt\n' * 4 + '300'

test = Test(code, expected)

if __name__ == '__main__':
    main(test)
