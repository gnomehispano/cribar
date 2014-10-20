''' Enviornment file for Behave tests '''

from dogtail.utils import checkForA11y
from dogtail.utils import GnomeShell
import os
import signal

APP_BINARY = 'cribar'


def before_all(context):    
    context.app_id = None
    checkForA11y()
    context.binary = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                  '..',
                                  'src',
                                  APP_BINARY)
    context.gs = GnomeShell()


def after_scenario(context, scenario):
    if context.app_id != None and scenario.status == 'failed':
        os.kill(context.app_id, signal.SIGTERM)
