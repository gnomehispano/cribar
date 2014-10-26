from behave import *
import os

@given(u'the Inbox folder have some photos')
def step_impl(context):
    from_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'test_images', '*')
    context.inbox_path = os.path.join(os.environ['HOME'], 'Inbox')
    os.system('mkdir -p ' + context.inbox_path)
    os.system('cp ' + from_path + ' ' + context.inbox_path)


@when(u"the user click on 'Accept'")
def step_impl(context):
    accept_button = context.main_window.child('Accept')
    accept_button.click()


@when(u"the user click on 'Trash'")
def step_impl(context):
    accept_button = context.main_window.child('Accept')
    accept_button.click()


@then(u"the main window title is '2874984824_5b11dd69af_o.jpg'")
def step_impl(context):
    assert context.main_window.name == '2874984824_5b11dd69af_o.jpg'
    os.system('rm -f ' + os.path.join(context.inbox_path, '*'))
    context.execute_steps(u'''
    When the user quit the application
    Then the application is stopped
    ''')


@then(u"the photo has moved to 'Pictures' folder")
def step_impl(context):
    photo_file = os.path.join(os.environ['HOME'], 'Pictures', '2874984824_5b11dd69af_o.jpg')
    assert os.path.isfile(photo_file)
    os.system('rm -f ' + os.path.join(context.inbox_path, '*'))
    os.system('rm -f ' + photo_file)
    context.execute_steps(u'''
    When the user quit the application
    Then the application is stopped
    ''')
