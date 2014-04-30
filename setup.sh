rm overlay
touch overlay
python -m SimpleHTTPServer &> /dev/null &
node playground.js &
sleep 1
./a.out image_dir ref_images &
sleep 1

trap "kill 0" SIGINT SIGTERM

sleep 50000
