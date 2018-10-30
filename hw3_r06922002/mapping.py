# coding=big5
import codecs
import sys

# Get input/output filename from arguments.
input_filename = sys.argv[1]
output_filename = sys.argv[2]

# Key-value pairs for ZhuYin-big5.
ZhuYin_big5_dict = dict()

# Reading Big5-ZhuYin.map...
with codecs.open(input_filename, 'r', encoding='big5-hkscs', errors='ignore') as file:
    for line in file:
        # The first element is Big5, and the second are ZhuYins.
        big5 = line.split(' ')[0]
        ZhuYins = line.split(' ')[1].split('/')

        # Only use initials for mapping.
        initials = list(set([x[0] for x in ZhuYins]))

        # Mapping each initial to big5(s).
        for initial in initials:
            # The initial already exists in dict, append big5 in value field.
            if initial in ZhuYin_big5_dict:
                ZhuYin_big5_dict[initial].append(big5)
            # The initial is new coming, put key and value in dict.
            else:
                ZhuYin_big5_dict[initial] = [big5]
        # After ZhuYin-big5 mapping, append big5-big5 mapping.
        ZhuYin_big5_dict[big5] = big5

# Writing ZhuYin-Big5.map...
with codecs.open(output_filename, 'w', encoding='big5-hkscs', errors='ignore') as file:
    for key in ZhuYin_big5_dict:
        # Each line's format is 'key values'.
        file.write(key + ' ')
        for index, big5 in enumerate(ZhuYin_big5_dict[key]):
            # Insert space (' ') between values (big5).
            if index == (len(ZhuYin_big5_dict[key]) - 1):
                file.write(big5 + '\n')
            else:
                file.write(big5 + ' ')