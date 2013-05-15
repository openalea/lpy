from openalea.vpltk.qt import qt
from settings import getSettings

try :
    import pysvn
    has_svn = True
except ImportError, e:
    has_svn = False

QMessageBox = qt.QtGui.QMessageBox

def hasSvnSupport():
    return has_svn

if has_svn:
    svn_client = None
    svn_client_gui_parent = None
    def get_svn_client():
        global svn_client
        if not svn_client : 
            svn_client = create_svn_client()
        return svn_client
    
    def create_svn_client():
        qpath = getSettings().fileName()
        import os
        settingpath = os.path.dirname(qpath)
        svnsettingpath = os.path.join(settingpath,'svn')
        if not os.path.exists(svnsettingpath):
            os.mkdir(svnsettingpath)
        client = pysvn.Client(svnsettingpath)
        client.set_store_passwords(True)
        client.set_auth_cache(True)
        
        def get_login( realm, username, may_save ):
            import logindialog
            dialog = qt.QtGui.QDialog(svn_client_gui_parent)
            widget = logindialog.Ui_LoginDialog()
            widget.setupUi(dialog)
            dialog.setWindowTitle(realm)
            widget.loginEdit.setText(username)
            if dialog.exec_() != qt.QtGui.QDialog.Accepted:
                return False, '', '', False
            else:
                return True, str(widget.loginEdit.text()), str(widget.passEdit.text()), True
            
        def ssl_client_cert_password_prompt( realm, may_save ):
            import logindialog
            dialog = qt.QtGui.QDialog(svn_client_gui_parent)
            widget = logindialog.Ui_LoginDialog()
            widget.setupUi(dialog)
            dialog.setWindowTitle(realm)
            widget.loginLabel.setText('Domain')
            widget.loginEdit.setText(realm)
            if dialog.exec_() != qt.QtGui.QDialog.Accepted:
                return False, '', False
            else:
                return True,  str(widget.passEdit.text()), True
            
        #def callback_notify(event_dict):
        #    client.parent.lpystudio.statusBar().showMessage(str(event_dict))
         
        def ssl_server_trust_prompt(trust_dict ):
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

    def svnUpdate(parent, fname ):
        client = get_svn_client()
        svn_client_gui_parent = parent
        import os
        fname = os.path.abspath(fname)
        local_rev = client.info2(fname)[0][1]['rev']
        try:
            rev = client.update(fname)
            if type(rev) is list: rev = rev[0]
            if local_rev.number == rev.number:
                QMessageBox.question(parent,'Update', 'Your version is already up-to-date : %s' % rev.number)
                return False
            else:
                QMessageBox.question(parent,'Update', 'Updated at revision %s' % rev.number)
                return True
        except pysvn.ClientError, ce:
            QMessageBox.warning(parent,'Update', ce.message)
            return False
        
    def svnIsUpToDate(parent, fname):
        client = get_svn_client()
        svn_client_gui_parent = parent
        import os
        fname = os.path.abspath(fname)
        local_entry_list = client.info2(fname)[0]
        current_rev = local_entry_list[1]['rev']
        try:
            server_entry_list = client.info2(fname,revision = pysvn.Revision( pysvn.opt_revision_kind.head ))[0]
            server_rev = server_entry_list[1]['last_changed_rev']
            if current_rev.number < server_rev.number:
                changelogs = client.log(fname,revision_start = server_rev, revision_end = current_rev)
                msg = 'A new version of the model exists : %s (current=%s).\n' % (server_rev.number,current_rev.number)
                for log in changelogs:
                    msg += " - [%s][%s] '%s'\n" % (log.revision.number,log.author,log.message)
                if isSvnModifiedFile(fname):
                    msg += "Warning : You also modified the file."
                QMessageBox.question(parent,'Up-to-date',msg )
                return False
            else:
                msg = 'Your version is up-to-date (current=%s).\n' % (current_rev.number)
                if isSvnModifiedFile(fname):
                    msg += "You modified the file."
                QMessageBox.question(parent,'Up-to-date', msg)
                
                return True
        except pysvn.ClientError, ce:
            QMessageBox.warning(parent,'Up-to-date', ce.message)
            return True

    def svnFileStatus(fname):
        import os
        fname = os.path.abspath(fname)
        client = get_svn_client()
        cwd = os.getcwd()
        os.chdir(os.path.dirname(fname))
        res = client.status(fname)[0]
        os.chdir(cwd)
        return res

    def svnFileTextStatus(fname):
        return svnFileStatus(fname).text_status

    def isSvnFile(fname):
        try:
            res = svnFileTextStatus(fname)
            return (res !=  pysvn.wc_status_kind.unversioned and res !=  pysvn.wc_status_kind.none)
        except pysvn.ClientError,e:
            return False
        
    def isSvnModifiedFile(fname):
        try:
            res = svnFileTextStatus(fname)
            return (res ==  pysvn.wc_status_kind.modified)
        except pysvn.ClientError,e:
            return False
    
    for d in dir(pysvn.wc_status_kind):
        globals()[d] = getattr(pysvn.wc_status_kind,d)