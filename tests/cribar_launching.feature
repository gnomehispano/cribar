# Cribar
# Test author: Álvaro Peña <alvaropg@gmail.com>

Feature: Application launch

@launching
Scenario: Launch and quit Cribar
         Given the application is launched
         And the main window is presented
         When the user quit the application
         Then the application is stopped
