# Sistema de Monitoreo Electrónico para Animales (V1 - 2016)

Este proyecto corresponde a la versión inicial desarrollada como trabajo de titulación para la carrera de Técnico Universitario en Telecomunicaciones y Redes (UTFSM, 2016). El sistema permite mantener un seguimiento en tiempo real de animales mediante el uso de tecnología GPS y envío de alertas SMS.

## Descripción del Proyecto
El dispositivo fue diseñado para detectar cuando un animal sale de un "área segura" predefinida. Al detectar esta intrusión, el sistema calcula la ubicación exacta y envía un mensaje de texto (SMS) al usuario con las coordenadas y un enlace directo a Google Maps.

![Prototipo Físico 2017](ruta_de_tu_imagen_1.jpg)

## Arquitectura del Sistema
El sistema se basa en un microcontrolador que gestiona la comunicación entre el módulo GPS (para posicionamiento) y el módulo GPRS/GSM (para la transmisión de datos por red celular).

### Componentes principales:
* **Microcontrolador:** Arduino Uno.
* **Posicionamiento:** Módulo GPS (DFRobot TEL0094).
* **Comunicación:** Módulo GPRS/GSM (SIM900).
* **Alimentación:** Batería portátil de alta capacidad.

![Diagrama de Conexiones](ruta_de_tu_imagen_2.jpg)

## Funcionamiento
1. **Detección:** El sistema compara constantemente la coordenada actual del GPS con la coordenada del "área segura".
2. **Cálculo de Distancia:** Si la distancia supera el límite configurado (`area_segura` + `tolerancia`), se activa la alerta.
3. **Transmisión:** El Arduino envía comandos AT al módulo GSM para establecer comunicación y enviar el SMS con el formato:
    * Fecha y hora del evento.
    * Latitud y Longitud.
    * Distancia respecto al área segura.
    * Enlace de Google Maps.

## Código Fuente
El código original (`RastreadorV1.ino`) utiliza la librería `TinyGPS` para el procesamiento de datos NMEA.

> **Nota:** Este código fue diseñado para trabajar exclusivamente en redes 2G. Actualmente, esta versión se mantiene como base histórica.

## Roadmap hacia V2 (Proyecto Actual)
Actualmente, el proyecto se encuentra en fase de renovación tecnológica:
* **Hardware:** Migración de Arduino Uno a **ESP32**.
* **Conectividad:** Transición de SMS (2G) a servicios IoT basados en **Wi-Fi**.
* **Software:** Actualización de librerías a **TinyGPS++** y modernización del entorno de desarrollo.

## Documentación
Para mayor detalle sobre los cálculos de diseño y pruebas de campo, consultar el documento: `Documentacion.pdf`.
