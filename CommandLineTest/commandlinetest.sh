#!/bin/bash

USERNAMES=usernames.csv
PASSWORDS=passwords.csv
QUESTIONS=question_bank.txt
ANSWERS=Answers.txt
USERANS=userans.txt
RESULTS=results.txt
SCORECARD=dispscore.txt

function displayResults()
{
	echo -e "\e[1;34m######################################################################\e[0m\n"
	echo -e "\e[1;33m*************************\e[0m  RESULT TIME\e[0m  \e[1;33m******************************\e[0m\n"
	echo -e "\e[1;34m######################################################################\e[0m"

	sleep 2
	clear
	truncate -s 0 $SCORECARD

	echo -e "       \e[1;33m************************************************************\e[0m       " >> $SCORECARD
	echo -e "                        *** You Scored \e[1;31m$1/10\e[0m ***                        " >> $SCORECARD
	echo -e "       \e[1;33m************************************************************\e[0m       " >> $SCORECARD
	echo -e "Use \e[1;31mup\e[0m and \e[1;31mdown\e[0m arrow keys to navigate and \e[1;31mq\e[0m to quit" >> $SCORECARD
	echo "-------------------------------------------------------------------------" >> $SCORECARD

	red_start="\e[1;31m"
	red_end="\e[0m"
	green_start="\e[1;32m"
	green_end="\e[0m"
	
	for i in {1..10}
	do
		echo "$(sed -n "$((i * 5 - 4))p" $QUESTIONS) : status : $(sed -n "${i}p" $RESULTS)" >> $SCORECARD
		head -$((i * 5)) $QUESTIONS | tail -4 >> $SCORECARD
	done

	sed -i 's/\r//g' $SCORECARD

	less -R $SCORECARD

	echo "Done!!!"
	sleep 3
}

function calculateScore()          #Check once
{
	score=0 
	truncate -s 0 $RESULTS

	for i in {1..10}
	do
		char1=$(cut -c1 $USERANS | head -$i | tail -1)
		char2=$(cut -c1 $ANSWERS | head -$i | tail -1)
		if [ "$char1" == "$char2" ]
		then
			score=$((score + 1))
			echo -e "\e[1;31mRight\e[0m" >> $RESULTS
		else
			echo -e "\e[1;32mWrong\e[0m" >> $RESULTS
		fi
	done

	sleep 2
	displayResults $score
}

function takeTest()
{
    clear
    echo -e "\e[1;33m**********************************************************************\e[0m"
    echo -e "                          \e[1;31mBEST OF LUCK\e[0m                                "
    echo -e "\e[1;33m**********************************************************************\e[0m"
    sleep 2

    truncate -s 0 $USERANS

    for i in `seq 1 10`
    do
    {
        clear
        echo -e "\e[1;31m-----------------------------------------------------------------\e[0m"
        head -$((i * 5)) $QUESTIONS | tail -5
        echo -e "\e[1;33m-----------------------------------------------------------------\e[0m"

        for j in `seq 10 -1 1`
        do
            echo -n -e "\rYou have \e[1;31m$j\e[0m seconds to enter your answer:\c"
            read -t 1 opt
            if [ ${#opt} -ne 0 ]
            then
    	        break
            fi
        done

        echo "$opt" >> $USERANS
    }
    done

    clear
    echo -e "\e[1;31m**********************************************************************\e[0m"
    echo -e "                          \e[1;33mTEST COMPLETE\e[0m                               "
    echo -e "\e[1;31m**********************************************************************\e[0m"
    sleep 2
	clear
	calculateScore
    sleep 2

	clear
}

function loggedIn()
{
    echo -e "\e[1mWould you like to take the test?\e[0m\n\t\t1-Take test\n\t\t2-Exit to Main Menu(Signout)"
	read opt

	if [ $opt -eq 1 ]
	then
		#Take test
        takeTest
	else if [ $opt -eq 2 ]
	then
		#Exit to Main Menu
        echo "Exit to Main Menu"
	else
        echo "Incorrect option!!!"
		exit
    fi
	fi
}
function signUp()
{
	echo -e "\e[1mUsername:\e[0m"
	read usrname
	#res=${#(grep "$usrname" $USERNAMES)}
	if grep -q "$usrname" $USERNAMES
	then
		{
			echo "Error: Username exist!!!"
			exit
		}
	fi

	echo -e "\e[1mEnter Password:\e[0m"
	read -s passwd
	if [ ${#passwd} -lt 8 ]
	then
	{
		echo "Password has less than 8 characters"
		exit
	}
	fi

	echo -e "\e[1mRe-Enter Password:\e[0m"
	read -s repass
	if [ "$repass" == "$passwd" ]
	then
	{
		# Appending username
		echo "$usrname" >> $USERNAMES

		#Appending password
		echo "$passwd" >> $PASSWORDS
	}
    else
	{
	    echo "Error: Passwords not matching!!!"
	    exit
	}
	fi
}

function signIn()
{
	echo -e "\e[1mUsername:\e[0m"
	read usrname

	res=`grep -n "$usrname" $USERNAMES`
	if [ ${#res} -z ]
	then
		echo "Username doesn't exist"
		exit
	fi

	pos=${res:0:1}
	echo -e "\e[1mPassword:\e[0m"
	read -s passwd

	if [ "$passwd" != "$(sed -n $pos'p' $PASSWORDS)" ]
	then
		echo "Error: Incorrect Password!!!"
		echo "Failed SignIn"
		exit
	else
    	echo -e "\e[1;34mSignIn Successful\e[0m"
        loggedIn
    fi
}

choice=0

while [ $choice -ne 3 ]
do
{
    echo -e "\e[1mPlease select an option :\e[0m\n\t\t1-Signup\n\t\t2-Signin\n\t\t3-Exit\n"
    read choice
	if [ $choice -eq 1 ]
	then
	{
        #SignUp
		echo -e "You chose to \e[1;32msignup\e[0m"
		signUp
	}
	elif [ $choice -eq 2 ]
	then
	{   
        #SignIn
		echo -e "You chose to \e[1;32msignin\e[0m"
		signIn
	}
	elif [ $choice -eq 3 ]
	then
		echo -e "You chose to \e[1;32mexit\e[0m"
	else
		echo "Error : Invalid option!!!"
	fi
}
done
exit