# Release Notes for STM32U3-2M-GettingStarted-AudioEventDetection Application

## Purpose

This project provides an STM32 Microcontroler embedded real time environement
to execute
[STEdgeAI core](https://www.st.com/en/development-tools/stedgeai-core.html)
generated model targetting audio applications. The purpose of this package is
to stream physical data acquired by sensors into a processing chain including a
preprocessing step that typically would perform a first level of feature
extraction, the machine learning inference itself, and a post processing step
before exposing the results to the user in real time.

## Key Features

- Support of Audio Event Detection (AED) using Yamnet 256 on STM32U3-2M

## Software components

| Name                                | Version     | Release notes
|-----                                | -------     | -------------
| STEdge AI runtime                   |  v4.0.0     |
| stm32-audio-sensing-processing      |  v1.5.0     | [release notes](Middlewares/ST/stm32-audio-sensing-processing/Release_Notes.html)
| CMSIS                               |  v5.9.0     | [release notes](Drivers/CMSIS/st_readme.txt)
| STM32U3xx CMSIS                     |  V1.3.0     | [release notes](Drivers/CMSIS/Device/ST/STM32U3xx/Release_Notes.html)
| STM32U3xx_HAL_Drivers               |  V1.3.0     | [release notes](Drivers/STM32U3xx_HAL_Driver/Release_Notes.html)
| HSP engine                          |  V1.0.0     | [release notes](Middlewares/ST/hsp_engine/Release_Notes.html)

## Available documentation

- README.md

## Update history

### v1.0.0 / March 2026

Initial release
