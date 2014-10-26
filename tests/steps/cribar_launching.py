from behave import *
from dogtail import tree
from dogtail.utils import run


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
