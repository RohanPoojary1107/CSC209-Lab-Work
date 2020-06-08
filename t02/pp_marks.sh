#!/bin/bash
declare -i total_score=0
declare -i total_max=0
for d in {1..4}; do
	declare -i a_score=0
	declare -i a_max=0
	if [ -d "a$d" ] && [ -e "a$d/feedback.txt" ]; then
		grade=`tail -n1 "a$d/feedback.txt" | tr -d [:blank:]`
		IFS='/' read -ra grArray <<< "$grade"
		a_score=${grArray[0]}
		a_max=${grArray[1]}
		total_score+=$a_score
		total_max+=$a_max
		echo "a$d: $a_score / $a_max"
	else
		echo "a$d: - / -"
	fi
done

echo

declare -i tut_total_score=0
declare -i tut_total_max=0
for d in {01..11}; do
        declare -i t_score=0
        declare -i t_max=0
        if [ -d "t$d" ] && [ -e "t$d/feedback.txt" ]; then
                grade=`tail -n1 "t$d/feedback.txt" | tr -d [:blank:]`
                IFS='/' read -ra grArray <<< "$grade"
                t_score=${grArray[0]}
                t_max=${grArray[1]}
                tut_total_score+=$t_score
                tut_total_max+=$t_max
                echo "t$d: $t_score / $t_max"
        else
                echo "t$d: - / -"
        fi
done

echo

echo "Assignment Total: $total_score / $total_max"
echo "Tutorial Total: $tut_total_score / $tut_total_max"

