// routes/mqttRoutes.js
const express = require('express');
const { check, validationResult } = require('express-validator');
const { publishMessage } = require('../config/mqttClient');
const router = express.Router();

router.post(
    '/publish',
    [
        check('topic', 'Topic is required').not().isEmpty(),
        check('message', 'Message is required').not().isEmpty(),
    ],
    (req, res) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) {
            return res.status(400).json({ errors: errors.array() });
        }

        const { topic, message } = req.body;

        publishMessage(topic, message);

        res.status(200).json({ msg: `Message envoyé sur le topic ${topic}` });
    }
);

module.exports = router;
