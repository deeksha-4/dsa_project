# Open a file for writing
with open("../outputs/data12.txt", "w") as file:
    # The line you want to write
    # line = "ASML 650 s#\nASML 645 b#\nAAPL 430 b#\nAAPL 435 b#\nAAPL 435 s#\nASML 646 b#\nASML 647 b#\nASML 651 b#\nAAPL 432 s#\nASML 654 s#\nASML 652 s#\nASML 650 s#\n"
    # line = "No Trade\nNo Trade\nNo Trade\nAAPL 435 s\nNo Trade\nNo Trade\nNo Trade\nASML 651 s\nAAPL 432 b\nNo Trade\nNo Trade\nASML 650 b\n"

    line = "ASML 650 s#\nASML 645 b#\nAAPL 430 b#\nAAPL 435 b#\nAAPL 435 s#\nASML 646 b#\nASML 647 b#\nASML 651 b#\nAAPL 432 s#\nASML 654 s#\nASML 652 s#\nASML 650 s#\n"
    
    # Write the line 50 times
    # for _ in range(48):
    file.write(line)