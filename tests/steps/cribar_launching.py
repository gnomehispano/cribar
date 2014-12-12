from behave import *
from dogtail import tree
from dogtail.utils import run
import os


@given(u'the application is launched')
def step_impl(context):
    context.app_id = run(context.binary, timeout=1)
    try:
        context.app = tree.root.application('cribar')
    except tree.SearchError:
        assert False, 'Cannot find the application'


@given(u'the main window is presented')
def step_impl(context):
    try:
        context.main_window = context.app.child(roleName='frame')
    except tree.SearchError:
        assert False, 'Cannot find the main window'


@given(u'the Inbox folder have not photos')
def step_impl(context):
    os.system("rm " + os.path.join(os.environ['HOME'], 'Inbox', '*'))


@when(u'the user quit the application')
def step_impl(context):
    context.gs.getApplicationMenuButton('Cribar').click()
    items = context.gs.getApplicationMenuList()
    for it in items:
        if it.name == 'Quit':
            it.click()


@then(u'the application is stopped')
def step_impl(context):
    try:
        app = tree.root.application(APP_NAME, False)
    except:
        app = None

    assert app is None, "Appliation not closed"


@then(u'the {button} button is unsensitive')
def step_impl(context, button):
    accept_button = context.main_window.child(button)
    assert accept_button.sensitive == False, "The button is sensitive"
    context.execute_steps(u'''
    When the user quit the application
    Then the application is stopped
    ''')
