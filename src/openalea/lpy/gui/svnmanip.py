from openalea.plantgl.gui.qt import qt
from openalea.plantgl.gui.qt.QtWidgets import QDialog, QMessageBox

from .settings import getSettings

try :
    import pysvn
    has_svn = True
except ImportError as e:
    has_svn = False


def hasSvnSupport():
    return has_svn

if has_svn:
    
    for n in ['modified', 'normal', 'conflicted', 'added']:
        globals()[n] = getattr(pysvn.wc_status_kind,n)

    svn_client = None
    svn_client_gui_parent = None
    svn_silent_client = None

    def get_svn_client():
        global svn_client
        if not svn_client : 
            svn_client = create_svn_client()
        return svn_client
    
    def get_svn_silent_client():
        global svn_silent_client
        if not svn_silent_client : 
            svn_silent_client = create_svn_silent_client()
        return svn_silent_client
    
    def create_svn_client():
        qpath = str(getSettings().fileName())
        import os
        settingpath = os.path.dirname(qpath)
        svnsettingpath = os.path.join(settingpath,'svn')
        if not os.path.exists(svnsettingpath):
            os.makedirs(svnsettingpath)
        client = pysvn.Client(svnsettingpath)
        client.set_store_passwords(True)
        client.set_auth_cache(True)
        
        def get_login( realm, username, may_save ):
            if svn_client_gui_parent is None : 
                print('Login is None')
                return False, '', '', False
            from . import logindialog
            dialog = QDialog(svn_client_gui_parent)
            widget = logindialog.Ui_LoginDialog()
            widget.setupUi(dialog)
            dialog.setWindowTitle(realm)
            widget.loginEdit.setText(username)
            if dialog.exec_() != QDialog.Accepted:
                return False, '', '', False
            else:
                return True, str(widget.loginEdit.text()), str(widget.passEdit.text()), True
            
        def ssl_client_cert_password_prompt( realm, may_save ):
            if svn_client_gui_parent is None : return False, '', False
            from . import logindialog
            dialog = QDialog(svn_client_gui_parent)
            widget = logindialog.Ui_LoginDialog()
            widget.setupUi(dialog)
            dialog.setWindowTitle(realm)
            widget.loginLabel.setText('Domain')
            widget.loginEdit.setText(realm)
            if dialog.exec_() != QDialog.Accepted:
                return False, '', False
            else:
                return True,  str(widget.passEdit.text()), True
            
        #def callback_notify(event_dict):
        #    client.parent.lpystudio.statusBar().showMessage(str(event_dict))
         
        def ssl_server_trust_prompt(trust_dict ):
            if svn_client_gui_parent is None: return True, True, True
            msg = 'The authenticity of host "%s" can\' t be established.\nRSA key fingerprint is %s.\nValid from %s to %s.\nCertified by %s.\nAccept ?'
            msg = msg % (trust_dict['hostname'], trust_dict['finger_print'],trust_dict['valid_from'],trust_dict['valid_until'],trust_dict['issuer_dname'])
            ok = QMessageBox.question(svn_client_gui_parent,'RSA Authentification of '+trust_dict['realm'], msg, QMessageBox.Ok,QMessageBox.Cancel )
            return ok == QMessageBox.Ok, True, True
        
        # client.callback_cancel
        # client.callback_get_log_message
        client.callback_get_login = get_login
        # client.callback_notify = callback_notify
        # client.callback_ssl_client_cert_prompt 
        client.callback_ssl_client_cert_password_prompt = ssl_client_cert_password_prompt
        client.callback_ssl_server_trust_prompt = ssl_server_trust_prompt        
        return client

    def create_svn_silent_client():
        qpath = getSettings().fileName()
        import os
        settingpath = os.path.dirname(qpath)
        svnsettingpath = os.path.join(settingpath,'svn')
        if not os.path.exists(svnsettingpath):
            os.mkdir(svnsettingpath)
        client = pysvn.Client(svnsettingpath)
        client.set_store_passwords(False)
        client.set_auth_cache(True)
        client.set_interactive(False)
        
        def get_login( realm, username, may_save ):
            return False, '', '', False

        
        def ssl_server_trust_prompt(trust_dict ):
            return True, True, True
        
        # client.callback_cancel
        # client.callback_get_log_message
        client.callback_get_login = get_login
        # client.callback_notify = callback_notify
        # client.callback_ssl_client_cert_prompt 
        client# .callback_ssl_client_cert_password_prompt = ssl_client_cert_password_prompt
        client.callback_ssl_server_trust_prompt = ssl_server_trust_prompt        
        return client

    def svnUpdate(fname, parent = None):
        global svn_client_gui_parent
        client = get_svn_client()
        svn_client_gui_parent = parent
        import os
        fname = os.path.abspath(fname)
        local_rev = client.info2(fname)[0][1]['rev']
        try:
            rev = client.update(fname)
            if type(rev) is list: rev = rev[0]
            if local_rev.number == rev.number:
                if parent : QMessageBox.question(parent,'Update', 'Your version is already up-to-date : %s' % rev.number)
                return False
            else:
                if parent : QMessageBox.question(parent,'Update', 'Updated at revision %s' % rev.number)
                return True
        except pysvn.ClientError as ce:
            QMessageBox.warning(parent,'Update', ce.message)
            return False

    def svnFileAdd(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.add(fname,False,False,False)
        os.chdir(cwd)
        return res
    
    def get_log( parent , title = 'SVN Commit'):
            from . import logdialog
            dialog = QDialog(parent)
            widget = logdialog.Ui_LogDialog()
            widget.setupUi(dialog)
            dialog.setWindowTitle(title)
            if dialog.exec_() != QDialog.Accepted:
                return False, ''
            else:
                return True,  str(widget.logEdit.toPlainText())
    
    def svnFileCommit(fname, msg = None, parent = None):
        import os
        fname = os.path.abspath(fname)
        if not isSvnAddedFile(fname) and not svnIsUpToDate(fname, parent, True):
            QMessageBox.question(parent,'Update first', repr(os.path.basename(fname))+' is not up to date.\nUpdate first.')
            return
        if msg is None :
            if parent is None:
                msg = ''
            else:
                ok, msg = get_log(parent, 'SVN Commit - '+os.path.basename(fname))
                if not ok: return
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.checkin(fname,msg)
        os.chdir(cwd)
        return res
        
    def svnFileRevert(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.revert(fname)
        os.chdir(cwd)
        return res
        
    def svnIsUpToDate( fname, parent = None, silent = False):
        import os
        global svn_client_gui_parent
        client = get_svn_client()
        svn_client_gui_parent = parent
        import os
        fname = os.path.abspath(fname)
        local_entry_list = client.info2(fname)[0]
        current_entry = local_entry_list[1]
        current_rev = current_entry['rev']
        try:
            server_entry_list = client.info2(fname,revision = pysvn.Revision( pysvn.opt_revision_kind.head ))[0]
            server_entry = server_entry_list[1]
            server_rev = server_entry['last_changed_rev']
            if current_rev.number < server_rev.number:
                if not silent and parent:
                    changelogs = client.log(fname,revision_start = server_rev, revision_end = current_rev)
                    msg = os.path.basename(fname) +'\nA new version of the model exists : %s (current=%s).\n' % (server_rev.number,current_rev.number)
                    for log in changelogs:
                        msg += " - [%s][%s] '%s'\n" % (log.revision.number,log.author,log.message)
                    msg += "Status : "+str(svnFileTextStatus(fname))
                    QMessageBox.question(parent,'Up-to-date',msg )
                return False
            else:
                if not silent and parent:
                    msg = os.path.basename(fname) +'\nYour version is up-to-date.\nRevision: %s.\n' % (current_rev.number)
                    if server_entry['last_changed_date']:
                        import time
                        msg += 'Last changed date : %s\n' % time.asctime(time.gmtime(server_entry['last_changed_date']))
                    if server_entry['last_changed_author']:
                        msg += 'Last changed author : %s\n' % server_entry['last_changed_author']
                    msg += "Status : "+str(svnFileTextStatus(fname))
                    QMessageBox.question(parent,'Up-to-date', msg)                
                return True
        except pysvn.ClientError as ce:
            if not silent and parent: 
                QMessageBox.warning(parent,'Up-to-date', ce.message)
                return True
            else:
                raise ce

    def svnFileStatus(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.status(fname,recurse=False)
        if not res is None and res != []: 
            res = res[-1]
        if res == [] : res = None
        os.chdir(cwd)
        return res

    def svnFileInfo(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.info(fname)
        os.chdir(cwd)
        return res


    def svnFileServerInfo(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.info(fname,revision = pysvn.Revision( pysvn.opt_revision_kind.head ))
        os.chdir(cwd)
        return res

    def svnFileTextStatus(fname):
        st = svnFileStatus(fname)
        if not st: return pysvn.wc_status_kind.none
        return st.text_status

    def isSvnFile(fname):
        try:
            res = svnFileTextStatus(fname)
            return (res !=  pysvn.wc_status_kind.unversioned and res !=  pysvn.wc_status_kind.none and res !=  pysvn.wc_status_kind.ignored)
        except pysvn.ClientError as e:
            return False
        
    def isSvnModifiedFile(fname):
        try:
            res = svnFileTextStatus(fname)
            return (res ==  pysvn.wc_status_kind.modified)
        except pysvn.ClientError as e:
            return False
    
    def isSvnAddedFile(fname):
        try:
            res = svnFileTextStatus(fname)
            return (res ==  pysvn.wc_status_kind.added)
        except pysvn.ClientError as e:
            return False
    
    def isSSHRepository(fname):
        try:
            res = svnFileInfo(fname)
            return ('+ssh' in res.url)
        except pysvn.ClientError as e:
            return False
    
    for d in dir(pysvn.wc_status_kind):
        globals()[d] = getattr(pysvn.wc_status_kind,d)
