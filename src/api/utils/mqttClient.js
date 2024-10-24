// config/mqttClient.js
const mqtt = require('mqtt');

const mqttClient = mqtt.connect('mqtt://your-mqtt-broker-url', {
    clientId: 'nodejs_mqtt_client',
    username: 'your_username',  // Si requis
    password: 'your_password',  // Si requis
    clean: true
});

mqttClient.on('connect', () => {
    console.log('Connected to MQTT broker');

    mqttClient.subscribe('home/sensors/temperature', (err) => {
        if (err) {
            console.error('Erreur d\'abonnement:', err);
        } else {
            console.log('Abonné au topic: home/sensors/temperature');
        }
    });
});

mqttClient.on('error', (error) => {
    console.error('Erreur de connexion MQTT:', error);
    mqttClient.end();
});

mqttClient.on('message', (topic, message) => {
    const messageContent = message.toString();
    console.log(`Message reçu sur le topic ${topic}: ${messageContent}`);
});

mqttClient.on('message', async (topic, message) => {
    const messageContent = message.toString();
    console.log(`Message reçu sur le topic ${topic}: ${messageContent}`);

    const newData = new SensorData({
        topic: topic,
        message: messageContent,
        timestamp: new Date()
    });

    try {
        await newData.save();
        console.log('Données enregistrées dans la base de données');
    } catch (error) {
        console.error('Erreur d\'enregistrement dans la base de données:', error);
    }
});


const publishMessage = (topic, message) => {
    mqttClient.publish(topic, message, (err) => {
        if (err) {
            console.error('Erreur d\'envoi du message:', err);
        } else {
            console.log(`Message envoyé au topic ${topic}: ${message}`);
        }
    });
};

module.exports = { mqttClient, publishMessage };
