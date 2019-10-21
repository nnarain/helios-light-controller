import paho.mqtt.client as mqtt
from argparse import ArgumentParser
import logging

COLORS = {
    'red': {'r': 255, 'g': 0, 'b': 0},
    'green': {'r': 0, 'g': 255, 'b': 0},
    'blue': {'r': 0, 'g': 0, 'b': 255},
    'yellow': {'r': 255, 'g': 255, 'b': 0}
}

def on_connect(client, args, flags, rc):
    logging.info('MQTT Client Connected')

    cmd = args.command
    cmd_value = getattr(args, '{}_value'.format(cmd))

    if cmd == 'color':
        cmd_value = COLORS[cmd_value]

    logging.info('Commanding {}={}'.format(cmd, cmd_value))

    payload = {cmd: cmd_value}

    topic = args.topic
    logging.info('Publishing to {} with {}'.format(topic, payload))
    client.publish(topic, str(payload))

def on_message(client, userdata, msg):
    pass

def main(args):
    broker = args.broker
    port = args.port

    client = mqtt.Client(userdata=args)
    client.on_connect = on_connect

    logging.info('Connecting...')
    client.connect(broker, port)

    try:
        client.loop_forever()
    except KeyboardInterrupt:
        logging.info('Exiting')

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    parser = ArgumentParser()
    parser.add_argument('-b', '--broker', default='localhost', help='MQTT Broker')
    parser.add_argument('-p', '--port', default=1883, help='MQTT Broker Port')
    parser.add_argument('-t', '--topic', required=True, help='Light topic')

    subparsers = parser.add_subparsers(dest='command')

    state_cmd = subparsers.add_parser('state', help='State light state to ON/OFF')
    state_cmd.add_argument('state_value', choices=['ON', 'OFF'], help='state value')

    color_cmd = subparsers.add_parser('color', help='Set solid effect color')
    color_cmd.add_argument('color_value', choices=['red', 'green', 'blue', 'yellow'], help='Color value')

    effect_cmd = subparsers.add_parser('effect', help='Set the strip effect')
    effect_cmd.add_argument('effect_value', default='Static', help='Effect name')

    speed_cmd = subparsers.add_parser('speed', help='Set the animation speed')
    speed_cmd.add_argument('speed_value', help='Animation speed')

    brightness_cmd = subparsers.add_parser('brightness', help='Set light strip brightness')
    brightness_cmd.add_argument('brightness_value', help='Brightness value')

    args = parser.parse_args()

    main(args)
