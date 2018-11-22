from tests.utils import Test, main

code = '''
זוגיים = רשימה()
לכל א בתוך טווח(2000):
    אם א % 2 שווהל 0:
        זוגיים.הוסף(א)

פלוט זוגיים
'''

expected = '[' + ', '.join(str(i) for i in range(2_000) if not i % 2) + ']'

test = Test(code, expected)

if __name__ == '__main__':
    main(test)
