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

    logging.info('Processing command {}'.format(cmd))

    if cmd == 'state':
        value = args.state_value
        payload = {'state': value}
    elif cmd == 'color':
        value = args.color_value
        logging.info('Commanding color {}'.format(value))
        payload = {'color': COLORS[value]}

    topic = args.topic
    logging.info('Publishing to {} with {}'.format(topic, payload))
    client.publish(topic, payload)

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
    color_cmd.add_argument('color_value', choices=['red', 'green', 'blue', 'yello'], help='Color value')

    args = parser.parse_args()

    main(args)
