# Cribar
# Test author: Álvaro Peña <alvaropg@gmail.com>

Feature: Application launch

@launching
Scenario: Launch and quit Cribar
         Given the application is launched
         And the main window is presented
         When the user quit the application
         Then the application is stopped



@launching
Scenario: Unsensitive 'Accept' button
          Given the Inbox folder have not photos
          And the application is launched
          And the main window is presented
          Then the Accept button is unsensitive


@launching
Scenario: Unsensitive 'Trash' button
          Given the Inbox folder have not photos
          And the application is launched
          And the main window is presented
          Then the Trash button is unsensitive
