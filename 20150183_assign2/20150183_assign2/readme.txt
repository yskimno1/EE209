/* Assignment 2 */

Name : Kim Yunseong
studnet ID : 20150183

by doing this Assignment, I could understand pointer notation, and
the use of 'const'. It was quite confusing, but
I used it better than before.

First of all, I did str.c only using with pointer. It was quite hard.

Also, if we can limit, or know the memory size of pcDest,
it is possible to call assert to verify,
like doing assert(Memory_size_of_src > Memory_size_of_dest), logically.
However, we don't know the memory size of Dest by using only parameters
in StrCopy. So, we cannot do that in our assignment, but it is still
possible if we give size of memory by parameters.

This assigment was easier than first assignment, so it took about 6~7 hours.
Actually, I already know past assignment 2 at past semester.
There were StrJoin and StrReplace, so it was easy to do sgrep before.
However, in this semester, the assignment was only 2 parts, not 3 parts.
I think the assignment 2 of past semester was harder than this semester.

For me, I'm expecting assignment 3, because I didn't do assignment 3 at
past semester. I think I will do better than now.

In this Assignment, when I do Dodiff, if two files have different lines,
it prints out both different lines and error messages.

At first time, I could not do clearing stdout buffers, so
stdout already showed different lines. I thought that it is not the bug,
but my algorithm was just showing both error messages and different lines,
because I compared two files line-by-line, and compared line number
at the last, outside of the while loop.

However, I confirmed this, by opening each files twice. It compares the
number of lines and number of characters in each lines first. If there
are no errors, then comparing starts. It is quite different algorithm, but
I thought that it is the only way to only show error message
if there are some errors.
