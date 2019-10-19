import numpy as np
from scipy.stats import entropy # calculate kl divergence #http://scipy.github.io/devdocs/generated/scipy.stats.entropy.html
np.set_printoptions(precision=2)
# https://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
# Original Paper http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.367.5281&rep=rep1&type=pdf

deletion_cost = 1
insertion_cost = 1
substitution_cost = 1

def fisher_wagner(line1, line2):
    print("Fisher Wagner Distance: %s => %s"%(line1,line2))

    print("Operations:")
    print("\ti(X) means that character 'X' gets inserted in the resulting string at the current position")
    print("\tn(X,X) means that two charcters were equal, no operation required")
    print("\ts(X,Y) means that the character 'X' gets substituted with 'Y' at the current position ")
    print("\td(X) means that character 'X' gets deleted from the current position")
    print("\tNote: n(,), the noop for comparing empty string to empty string has been ommitted for clarity")
    # distance matrix
    d = np.zeros(shape=(len(line1)+1, len(line2)+1))
    ops = np.ndarray(shape=(len(line1)+1, len(line2)+1), dtype=np.dtype('S100') )
    ops[:] = ""  #clear array
    # ops[0,0] = "n( , )"

    for i in xrange(1,len(line1)+1):
        d[i,0] = d[i-1,0] + deletion_cost # the distance of any first string to an empty second string requires i deletions
        ops[i, 0] = "%sd(%c)"%(ops[i-1,0],line1[i-1])

    for j in xrange(1,len(line2)+1):
        d[0, j] = d[0,j-1] + insertion_cost# the distance of any second string to an empty first string
        ops[0,j] = "%si(%c)"%(ops[0,j-1],line2[j-1])

    print("Cost matrix for empty strings")
    print(d)
    print("Operation matrix for empty strings")
    print(ops)



    for j in xrange(1,len(line2)+1):
        for i in xrange(1,len(line1)+1):
            char1 = line1[i-1] # because strings in python are also zero indexed
            char2 = line2[j-1] # because strings in python are also zero indexed
            print("\nStep(i=%i,j=%i)"%(i, j))
            print("Goal: %s => %s"%(line1[0:i],line2[0:j]))
            print("Comparing: '%s' to '%s'"%(char1, char2))
            if char1 == char2:
                print("\t=> equal, no change required")
                d[i, j] = d[i-1, j-1]
                ops[i,j]="%sn(%c,%c)"%(ops[i-1, j-1],char1,char2)
            else:
                print("\t=> not equal, need to modify something")
                del_op_cost = d[i-1,j  ] + deletion_cost     # deletion
                ins_op_cost = d[i  ,j-1] + insertion_cost    # insertion
                subs_op_cost   = d[i-1,j-1] + substitution_cost # substitution_cost

                print("\tCost of reaching goal via del:  %sd(%c): %i"%(ops[i-1, j], char1, del_op_cost))
                print("\tCost of reaching goal via ins:  %si(%c): %i"%(ops[i ,j-1], char2, ins_op_cost))
                print("\tCost of reaching goal via sub:  %ss(%c,%c): %i"%(ops[i-1, j-1], char1, char2, subs_op_cost))

                # assign performed operation
                if del_op_cost<ins_op_cost and del_op_cost<subs_op_cost:
                    ops[i,j]="%sd(%c)"%(ops[i-1, j], char1)
                elif ins_op_cost<del_op_cost and ins_op_cost<subs_op_cost:
                    ops[i,j]="%si(%c)"%(ops[i ,j-1], char2)
                else: # substitution
                    ops[i,j]="%ss(%c,%c)"%(ops[i-1, j-1], char1, char2)
                d[i, j] = min(del_op_cost, ins_op_cost, subs_op_cost)

            print("Cost Matrix:")
            print(d)
            print("Traces Matrix:")
            print(ops)
    return d[len(line1), len(line2)]
print(fisher_wagner("ABV", "FV"))


#print(fisher_wagner("ABCD", "DABC"))
#print(fisher_wagner("AB123DA","AB543DA"))
