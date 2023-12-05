echo "Creating batch test file..."
pwd
cd ..
pwd
cd mysh
which ls
echo HelloWorld | grep World
echo PipingOverride > test_output.txt | cat
echo PipingOverride
cat test_output.txt
echo redirection_test > test_output.txt
cat test_output.txt
test -f test_output.txt
then echo FILE_EXISTS
else echo FILE_NOT_EXISTS
ls *.txt
cat test_output.txt
sleep 2
rm -f test_output.txt
echo Finished Running
exit
echo ThisShouldNotRun
