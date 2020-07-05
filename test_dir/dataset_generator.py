import os
import random

import requests


def load_words():
    if not os.path.exists("words_alpha.txt"):
        r = requests.get("https://github.com/dwyl/english-words/raw/master/words_alpha.txt")
        with open('words_alpha.txt', 'wb') as f:
            f.write(r.content)

    with open('words_alpha.txt') as word_file:
        valid_words = list(word_file.read().split())

    return valid_words


if __name__ == '__main__':
    english_words = load_words()

    for fs in [1,2,4,8]:
        random.seed(10)
        entropy = english_words[0:10000 * fs]
        file_count = 1000 * fs
        os.mkdir("dicts/high_entropy_xxl_t{}/".format(fs))
        for i in range(0, file_count):
            with open("dicts/high_entropy_xxl_t{}/file{}.txt".format(fs, i), "w+") as f:
                for i in range(1, random.randint(100, 50000)):
                    f.write(random.choice(entropy) + " ")
