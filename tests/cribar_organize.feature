Feature: Photo organization

@photo_load
Scenario: First photo load
          Given the Inbox folder have some photos
          And the application is launched
          And the main window is presented
          Then the main window title is '2874984824_5b11dd69af_o.jpg'


@accep_photo
Scenario: Accept photo
          Given the Inbox folder have some photos
          And the application is launched
          And the main window is presented
          When the user click on 'Accept'
          Then the photo has moved to 'Pictures' folder


@trash_photo
Scenario: Trash photo
          Given the Inbox folder have some photos
          And the application is launched
          And the main window is presented
          When the user click on 'Trash'
          Then the photo has been removed
