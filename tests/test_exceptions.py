from tests.utils import Test, main

code = '''
ר = רשימה()
לכל _ בתוך טווח(100):
    נסה:
        ר[10]
    תפוס __שגיאת_מיקום__ בתור ש:
        הדפס ש
מ = מילון()
לכל _ בתוך טווח(100):
    נסה:
        מ['hi!']
    תפוס __שגיאת_מפתח__ בתור ש:
        הדפס ש
'''

expected = "<mwp'a '__shgyat_mykwm__'>\n" * 100 + "<mwp'a '__shgyat_mptj__'>\n" * 100

test = Test(code, expected)

if __name__ == '__main__':
    main(test)
