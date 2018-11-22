from tests.utils import Test, main

code = '''
פעולה פיב(מ):
    אם מ קטןמ 3 החזר 1 אחרת:
        החזר פיב(מ-1) + פיב(מ-2)

פלוט פיב(18)
'''

test = Test(code, '2584', timeout=20)

if __name__ == '__main__':
    main(test)
