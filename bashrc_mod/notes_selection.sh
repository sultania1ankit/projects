X=$(zenity --list \
  --title="Cold Pigeon" \
  --text="The Cold Notes" \
  --width=1000 \
  --height=600 \
  --radiolist \
  --print-column=4 \
  --hide-column=4 \
  --column="Selection" --column="Class"  --column="Description" --column="filename"\
    FALSE "Self   " "Scratch Pad" "scratch_pad.notes" \
    FALSE "Self   " "2 Do" "2_do.notes" \
    FALSE "Self   " "Doubts" "doubts.notes" \
    FALSE "Theory   " "C Theory" "c_theory.notes" \
    FALSE "Theory   " "Embedded Theory" "embedded_theory.notes" \
    FALSE "Theory   " "PCB Theory" "pcb_theory.notes" \
    FALSE "Theory   " "Interviews" "interview.notes" \
    FASLE "Theory   " "RTOS Theory" "rtos_thoery.notes" \
    FALSE "Study   " "Physics" "physics.notes" \
    FALSE "Study   " "Algorithms" "algos.notes" \
    FALSE "Study   " "Techniques" "techniques.notes" \
    FALSE "Rulebook   " "Firmware Rules" "firmware_rules.notes" \
    FALSE "Rulebook   " "Hardware Rules" "hardware_rules.notes" \
    FALSE "Rulebook   " "PCB Rules" "pcb_rules.notes" \
    FALSE "Production   " "Components" "components.notes" \
    FALSE "Production   " "Seen Circuits" "seen_circuits.notes" \
    FALSE "Extra   " "Notes Syntax's" "notes.notes" \
    FLASE "Extra   " "TechSwarm Drones" "tech_swarm_drone.notes" \
    FALSE "Extra   " "Youtube" "yt.notes" \
    ); \
if [ $X ]
then
  subl /home/ankit_sultania/codes/P_channel_poet/$X
else
echo "No file Selected"
fi














