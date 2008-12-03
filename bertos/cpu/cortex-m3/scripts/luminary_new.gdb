target remote localhost:3333
monitor reset
monitor sleep 500
monitor poll
monitor soft_reset_halt
break main
load
continue
