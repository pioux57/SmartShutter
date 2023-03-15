// https://www.xanthium.in/cross-platform-serial-port-programming-tutorial-java-jdk-arduino-embedded-system-tutorial
// https://docs.oracle.com/javase/7/docs/api/javax/swing/JTextField.html#:~:text=JTextField%20is%20a%20lightweight%20component,is%20reasonable%20to%20do%20so.
// https://github.com/Fazecast/jSerialComm/wiki/Event-Based-Reading-Usage-Example

// git config --global alias.tree "log --oneline --graph --decorate --all

import javax.swing.*;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Line2D;
import java.util.Timer;
import java.util.TimerTask;
import groovy.json.JsonSlurper;
import com.fazecast.jSerialComm.*;

class SmartShutter implements ActionListener, ChangeListener {
    // Swing window
    static JFrame       frm;
    static CardLayout   main_layout;
    static JPanel       panel_main,panel_splash,panel_com,panel_param,panel_pdv,panel_lightning,panel_timelapse;
    static JLabel       lbl_splash,lbl_console,lbl_serial,lbl_com,lbl_param,lbl_pdv,lbl_lightning,lbl_param_version,lbl_param_pmax,lbl_param_vmax,lbl_sensibility,lbl_timelapse,lbl_tl_delay;
    static JTextArea    ta_console,ta_serial;
    static JScrollPane  sp_console,sp_serial;
    static JButton      btn_connect,btn_pdv,btn_detect,btn_tl_start;
    static JMenuBar     mb_menuBar;
    static JMenu        m_commands,m_communication;
    static JMenuItem    mi_communication,mi_lightning,mi_pdv,mi_parameters,mi_timelapse;
    static Dimension    panelSize;
    static Image        icon;
    static JTextField   tf_param_version,tf_param_pmax,tf_param_vmax,tf_tl_value,tf_tl_countdown;
    static JSlider      sl_sensibility;
    static JCheckBox    cb_sensibility;
    static JRadioButton rb_detectionMode0;
    static JRadioButton rb_detectionMode1;
    static JRadioButton rb_detectionMode2;
    static ButtonGroup  bg_detectionMode;
    static Timer        timer;
    static Timer        timerCheck;

    // Graph du mode lightning
    static int[]        g_values = new int[600];
    static int          lineIndex = 0;

    // Libelles
    static String languageToUse = "fr";
    static JsonSlurper jsonSlurper = new JsonSlurper();
    static Object lang;

    // Variables
    static boolean  debugMode = false;
    static boolean  simulationMode = false;
    static SerialPort[] serialList = SerialPort.getCommPorts();
    static SerialPort selectedPort;
    static String   layoutShown = "splash";
    static int      BaudRate = 9600;
    static int      DataBits = 8;
    static int      StopBits = SerialPort.ONE_STOP_BIT;
    static int      Parity   = SerialPort.NO_PARITY;
    static int      i = 0;
    static int      indexArduino = 0;
    static boolean  arduinoDetected = false;
    static boolean  smartShutterReady = false;
    static String   packet = "";
    static String   serialPacketLine = "";

    // Données du SmartShutter
    static String   ss_version;
    static int      ss_potentiometerMax;
    static int      ss_valueMax;
    static int      ss_potentiometerValue;
    
    // UI -> SmartShutter
    static int      ui_potentiometerValue;
    static boolean  ui_timelapseStarted = false;
    
    //----------------------------------------
    // Methode principale
    static void main(String[] args){
        SmartShutter s = new SmartShutter();
        s.lang = jsonSlurper.parse(new File("UI/language.json"));

        // Si on passe des parametres en entree on les gere ici
        for (i = 0 ; i < args.length ; i++){
            if (args[i] == "debug") {s.debugMode = true;}
            if (args[i].contains("lang")) {s.languageToUse = args[i].split(":")[1];}
        }

        // Initialisation de l'objet et sa fenetre
        s.initialiseFrame();
        if (s.debugMode) {
            String[] options = [s.lang."${s.languageToUse}".msgYes, s.lang."${s.languageToUse}".msgNo];
            int choice = JOptionPane.showOptionDialog(s.frm, s.lang."${languageToUse}".msgDebug, "DEBUG MODE",JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE,null, options, options[0]);
            if (choice == 0) {s.simulationMode = true;}
        }
        s.portAutoDetect();
    }

    //----------------------------------------
    // Initialise la fenetre
    public void initialiseFrame(){
        /*********************************************/
        // INITIALISATION
        /*********************************************/
        panelSize       = new Dimension(800,600);
        frm             = new JFrame("SmartShutter UI");
        icon            = Toolkit.getDefaultToolkit().getImage("img/logo_px_small.png");
        frm.setIconImage(icon);
        main_layout     = new CardLayout();

        panel_main      = new JPanel(main_layout);
        panel_splash    = new JPanel(null);
        panel_com       = new JPanel(null);
        panel_param     = new JPanel(null);
        panel_pdv       = new JPanel(null);
        panel_timelapse = new JPanel(null);
        panel_lightning = new JPanel(null){
            @Override
            public void paintComponent(Graphics g) {
                super.paintComponent(g);
                // Cadre du graph
                g.setColor(Color.BLACK);
                g.drawRect(99, 149, 601, 351);
                g.setColor(Color.WHITE);
                g.fillRect(100, 150, 600, 350);

                // Calcul de la sensibilité et dessin du niveau
                float ratioGraph = 350 / ss_valueMax;
                int sensibility = (int)((ss_valueMax * ratioGraph) * ((ss_potentiometerMax - sl_sensibility.getValue()) / ss_potentiometerMax));
                g.setColor(Color.MAGENTA);
                g.drawLine(100,499-sensibility,700,499-sensibility);

                // Affichage des valeurs stockées
                g.setColor(Color.BLUE);
                for (int j = 0 ; j < 600 ; j++) {
                    int value = (int)(g_values[j]*ratioGraph);

                    if (value > 350) {value = 350;}
                    if (value > sensibility) {g.setColor(Color.RED);} else {g.setColor(Color.BLUE);}
                    g.drawLine(100+j,499,100+j,499-value);
                }
                g.setColor(Color.GREEN);
                g.drawLine(100+lineIndex,499,100+lineIndex,150);

            }            
        }

        panel_splash.setMinimumSize(panelSize);
        panel_splash.setPreferredSize(panelSize);
        panel_com.setMinimumSize(panelSize);
        panel_com.setPreferredSize(panelSize);
        panel_param.setMinimumSize(panelSize);
        panel_param.setPreferredSize(panelSize);
        panel_pdv.setMinimumSize(panelSize);
        panel_pdv.setPreferredSize(panelSize);
        panel_lightning.setMinimumSize(panelSize);
        panel_lightning.setPreferredSize(panelSize);
        panel_timelapse.setMinimumSize(panelSize);
        panel_timelapse.setPreferredSize(panelSize);

        // Controles de l'UI de la frame
        mb_menuBar          = new JMenuBar();
        m_communication     = new JMenu(lang."${languageToUse}".menuCom);
        m_commands          = new JMenu(lang."${languageToUse}".menuControl);
        mi_communication    = new JMenuItem(lang."${languageToUse}".itemCom);
        mi_parameters       = new JMenuItem(lang."${languageToUse}".itemParam);
        mi_pdv              = new JMenuItem(lang."${languageToUse}".itemPDV);
        mi_lightning        = new JMenuItem(lang."${languageToUse}".itemLightning);
        mi_timelapse        = new JMenuItem(lang."${languageToUse}".itemTimelapse);

        // Construction du menu
        mb_menuBar.add(m_communication);
        mb_menuBar.add(m_commands);

        m_communication.add(mi_communication);
        m_communication.add(mi_parameters);

        m_commands.add(mi_pdv);
        m_commands.add(mi_lightning);
        m_commands.add(mi_timelapse);

        // Actions sur le menu
        mi_communication.addActionListener(this);
        mi_parameters.addActionListener(this);
        mi_lightning.addActionListener(this);
        mi_timelapse.addActionListener(this);
        mi_pdv.addActionListener(this);

        panelSplash();
        panelCom();
        panelParam();
        panelPDV();
        panelLightning();
        panelTimelapse();

        // AFFICHAGE DE LA FRAME
        frm.add(panel_main);
        
        panel_main.add("splash",panel_splash);
        panel_main.add("com",panel_com);
        panel_main.add("param",panel_param);
        panel_main.add("pdv",panel_pdv);
        panel_main.add("lightning",panel_lightning);
        panel_main.add("timelapse",panel_timelapse);
        
        frm.setJMenuBar(mb_menuBar);
        frm.setSize(800,600);
        frm.setLocationRelativeTo(null);
        frm.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frm.setVisible(true);

        main_layout.show(panel_main, "splash");
        refreshUIControls();

        frm.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                disconnectSerial();
                System.exit(0);
            }
        });
    }

    //----------------------------------------
    // Gestion des actions sur l'écran
    public void stateChanged(ChangeEvent c){
        switch(c.getSource()) {
            case sl_sensibility:
                if (cb_sensibility.isSelected() && !c.getSource().getValueIsAdjusting()) {setSensibility();}
            break;
        }
    }
    public void actionPerformed(ActionEvent e){
        switch(e.getSource()) {
            ///////////////////////////////////////
            // Changements de panels
            case mi_communication:
                if (selectedPort != null && selectedPort.isOpen()) {sendSerial(5);}  // Refus des données
                main_layout.show(panel_main, "com");
                layoutShown = "com";
            break;
            case mi_parameters:
                sendSerial(5);  // Refus des données
                main_layout.show(panel_main, "param");
                layoutShown = "param";
            break;
            case mi_pdv:
                sendSerial(5);      // Refus des données
                sleep(50);
                sendSerial(22);     // On passe le smartshutter en mode 2 (remote)

                main_layout.show(panel_main, "pdv");
                layoutShown = "pdv";
            break;
            case mi_lightning:
                sendSerial(6);      // Demande des données
                sleep(50);
                sendSerial(20);     // On passe le smartshutter en mode 0 (orage)

                main_layout.show(panel_main, "lightning");
                layoutShown = "lightning";
            break;
            case mi_timelapse:
                sendSerial(5);      // Demande des données
                sleep(50);
                sendSerial(21);     // On passe le smartshutter en mode 1 (timelapse)

                main_layout.show(panel_main, "timelapse");
                layoutShown = "timelapse";
            break;
            ///////////////////////////////////////
            // Action sur les controles
            case btn_connect:
                if (simulationMode) {connectSerial();}
                else {
                    if (selectedPort != null) {
                        if (selectedPort.isOpen()) { disconnectSerial();} else { connectSerial(); }
                    }
                }
            break;
            case btn_pdv:
                printConsole(lang."${languageToUse}".txtReq + "\n");
                if (smartShutterReady) {triggerShot();}
                else {printConsole(lang."${languageToUse}".txtSSNotReady + "\n");}
            break;
            case btn_detect:
                portAutoDetect();
            break;
            case cb_sensibility:
                sl_sensibility.enabled = cb_sensibility.isSelected();
                if (cb_sensibility.isSelected()) {
                    sendSerial(10);
                    sleep(50);
                    setSensibility();
                } else { sendSerial(11);}
            break;
            case rb_detectionMode0:
                if (rb_detectionMode0.isSelected()) sendSerial(30);
            break;
            case rb_detectionMode1:
                if (rb_detectionMode1.isSelected()) sendSerial(31);
            break;
            case rb_detectionMode2:
                if (rb_detectionMode2.isSelected()) sendSerial(32);
            break;
            case btn_tl_start:
                if (ui_timelapseStarted){
                    btn_tl_start.text = lang."${languageToUse}".btnStart;
                    ui_timelapseStarted = false;
                    tf_tl_value.enabled = true;
                    timer.cancel();
                    timerCheck.cancel();
                    tf_tl_countdown.text = "0";
                } else {
                    int delaySec = Integer.parseInt(tf_tl_value.getText());
                    if (delaySec > 0){
                        timer = new Timer();
                        timerCheck = new Timer();
                        btn_tl_start.text = lang."${languageToUse}".btnStop;
                        ui_timelapseStarted = true;
                        tf_tl_value.enabled = false;
                        timer.schedule(new taskTimeLapse(), delaySec*1000);
                        timerCheck.schedule(new taskCheck(), 1000);
                        tf_tl_countdown.text = delaySec;
                    } else {JOptionPane.showMessageDialog(frm, lang."${languageToUse}".msgZeroTL, "Timelapse", JOptionPane.ERROR_MESSAGE);}
                }
            break;
        }
    }

    //----------------------------------------
    // Autodetection du port sur lequel l'Arduino est connecté
    public void portAutoDetect(){
        arduinoDetected = false;

        disconnectSerial();
        i = 0;
        printConsole(lang."${languageToUse}".txtScanPort+"\n");
        printConsole("---------------------------------------\n");
        arduinoDetected = false;
        indexArduino = 0;
        serialList.each() { serial ->
            printConsole(serial.getDescriptivePortName() + " / " + serial.getPortDescription() + " / " + serial.getSystemPortName() + " -> ");
            if (serial.getPortDescription().contains("Arduino")){
                indexArduino = i;
                printConsole("OK\n");
                arduinoDetected = true;
            } else { printConsole("NO\n"); }
            i++;
        }
        printConsole("---------------------------------------\n");

        if (simulationMode){
            arduinoDetected = true;
            printConsole(lang."${languageToUse}".txtArduinoSimu+"\n");
        }
        else {
            if (arduinoDetected){
                selectedPort = serialList[indexArduino];
                selectedPort.setComPortParameters(BaudRate,DataBits,StopBits,Parity);         // Ajout des parametres de communication UART
                selectedPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 1000, 0);   // Timeout UART
                printConsole(lang."${languageToUse}".txtArduinoDetected+" [" + selectedPort.getSystemPortName() + "]\n");
            } else { 
                selectedPort = null;
                ta_console.setForeground(Color.RED);
                printConsole(lang."${languageToUse}".txtNoArduino+"\n");
                JOptionPane.showMessageDialog(frm, lang."${languageToUse}".msgNoArduino, lang."${languageToUse}".msgTitleAutoDetect, JOptionPane.ERROR_MESSAGE);
            }
        }

        printConsole("---------------------------------------\n");
        refreshUIControls();
    }

    //----------------------------------------
    // Connexion au SmartShutter
    public void connectSerial(){
        if (!simulationMode){
            if (selectedPort != null) {
                if (selectedPort.isOpen()) {disconnectSerial();}
                else {
                    selectedPort.openPort();
                    if (selectedPort.isOpen()){
                        printConsole(lang."${languageToUse}".txtConnected+" [" + selectedPort.getSystemPortName() + "]\n");
                        btn_connect.text = lang."${languageToUse}".btnDisconnect;

                        // Ecoute du port
                        printConsole(lang."${languageToUse}".txtDialIP+"\n");
                        selectedPort.addDataListener(new SerialPortDataListener() {
                            @Override
                            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
                            @Override
                            public void serialEvent(SerialPortEvent event) {
                                sleep(300);
                                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return;
                                
                                byte[] newData = new byte[selectedPort.bytesAvailable()];
                                int numRead = selectedPort.readBytes(newData, newData.length);

                                String currentByte;

                                // Lecture des octets recus
                                for (int i = 0; i < newData.length; ++i) {
                                    currentByte = (String)(char)newData[i];
                                    
                                    // Verifie si on est a la fin d'une ligne
                                    // Quand on a fini de lire une ligne complete on la traite
                                    if (currentByte == "\n"){
                                        if (layoutShown != "lightning") {printSerialConsole(serialPacketLine + "\n");}
                                        // On detecte si le SmartShutter a fini de démarrer et et prét é recevoir un ordre
                                        if (serialPacketLine.contains("[SMARTSHUTTER READY]")) {
                                            smartShutterReady = true;
                                            printConsole(lang."${languageToUse}".txtSMReady+"\n");
                                            sendSerial(18); // On previent le SmartShutter que le PC prend le controle
                                            JOptionPane.showMessageDialog(frm, lang."${languageToUse}".msgSMReady);
                                        } else {
                                            // Gestion des données du SmartShutter
                                            if (smartShutterReady) {receivedData(serialPacketLine);}
                                        }
                                        serialPacketLine = "";
                                    } else {serialPacketLine = serialPacketLine + currentByte;}
                                }
                                refreshUIControls();
                            }
                        });
                    } else {
                        printConsole(lang."${languageToUse}".txtConnectFailed+" [" + selectedPort.getSystemPortName() + "]\n");
                        JOptionPane.showMessageDialog(frm, lang."${languageToUse}".txtConnectFailed+" [" + selectedPort.getSystemPortName() + "]", lang."${languageToUse}".msgTitleConnect, JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
        } else {
            smartShutterReady = true;
            printConsole(lang."${languageToUse}".txtSMSimuReady+"\n");
            ss_version = "simu";
            ss_potentiometerMax = 940;
            ss_valueMax = 960;
            ss_potentiometerValue = 666;

        }
        refreshUIControls();
    }

    //----------------------------------------
    // Deconnexion du SmartShutter
    public void disconnectSerial(){
        if (selectedPort != null) {
            if (selectedPort.isOpen()){
                printConsole(lang."${languageToUse}".txtDisconnect+" [" + selectedPort.getSystemPortName() + "]\n");
                sendSerial(16); // On previent le SmartShutter qu'on se deconnecte
                sleep(500);
                selectedPort.closePort();
                btn_connect.text = lang."${languageToUse}".btnConnect;
                smartShutterReady = false;
                ta_serial.text = "";
            }
        }
    }

    //----------------------------------------
    // Envoi octet brut vers le SmartShutter
    public void sendSerialInt(int value){
        String valString = String.format("%d\n", value); // ptet ajouter un \n apres le %d... a tester
        sendSerialString(valString);
    }
    public void sendSerialString(String packet){
        try {
            byte[] valBytes = packet.getBytes();
            int bytesTxed  = 0;
        
            if (!simulationMode){
                bytesTxed = selectedPort.writeBytes(WriteByte,valBytes.length);
                if (bytesTxed == -1) {printSerialConsole(lang."${languageToUse}".txtUnableToSend+"\n");} else {printSerialConsole(lang."${languageToUse}".txtBytesSent +  bytesTxed + "\n");}
            }
        } catch (Exception error) { printConsole(error.getMessage() + "\n"); }
    }
    public void sendSerial(int octet){
        try {
            byte[] WriteByte = new byte[1];
            WriteByte[0] = octet;
            int bytesTxed  = 0;
        
            if (!simulationMode){
                bytesTxed  = selectedPort.writeBytes(WriteByte,1);
                if (bytesTxed == -1) {printSerialConsole(lang."${languageToUse}".txtUnableToSend+"\n");} else {printSerialConsole(lang."${languageToUse}".txtBytesSent +  bytesTxed + "\n");}
            }
        } catch (Exception error) { printConsole(error.getMessage() + "\n"); }
    }

    //----------------------------------------
    // Gestion des données reçues du SmartShutter
    private void receivedData(String packetReceived){
        String[] params = packetReceived.trim().split("=");
        switch(params[0]){
            case "v":
                ss_version = params[1];
                tf_param_version.text = ss_version;
            break;
            case "pmax":
                ss_potentiometerMax = Integer.parseInt(params[1]);
                tf_param_pmax.text = params[1];
                sl_sensibility.setMaximum(ss_potentiometerMax);
            break;
            case "vmax":
                ss_valueMax = Integer.parseInt(params[1]);
                tf_param_vmax.text = params[1];
            break;
            case "pvalue":
                ss_potentiometerValue = Integer.parseInt(params[1]);
                if (!cb_sensibility.isSelected()) {sl_sensibility.setValue(ss_potentiometerValue);}
            break;
            case "value":
                g_values[lineIndex] = Integer.parseInt(params[1]);
                // print(lineIndex);
                // print(":");
                // println(g_values[lineIndex]);
                lineIndex++;
                if (lineIndex == 600) {lineIndex = 0;}
                panel_lightning.repaint();
            break;
        }
    }

    //----------------------------------------
    // Demande d'une prise de vue
    public void triggerShot(){
        sendSerial(65); // Octet de prise de vue valeur 65
    }

    //----------------------------------------
    // Envoi la sensibilité séléctionnée au SmartShutter
    private void setSensibility(){
        sendSerial(4);  // On previent l'Arduino qu'on lui envoi un paquet derriére
        int sensibilityRnd = Math.round(sl_sensibility.getValue() / 10);
        sendSerial(sensibilityRnd);
    }

    //----------------------------------------
    // Rafraichissement des controles actifs ou non de l'écran
    private void refreshUIControls(){
        if (!debugMode){
            m_commands.enabled      = false;
            btn_connect.enabled     = false;
            btn_pdv.enabled         = false;
            mi_parameters.enabled   = false;

            if (arduinoDetected){
                btn_connect.enabled = true;

                if (smartShutterReady) {
                    m_commands.enabled      = true;
                    btn_pdv.enabled         = true;
                    mi_parameters.enabled   = true;
                }
            }
        }
    }

    //----------------------------------------
    // Affiche une chaine sur la console
    private void printConsole(String line){
        ta_console.append(line); 
        ta_console.setCaretPosition(ta_console.getDocument().getLength());
    }

    //----------------------------------------
    // Affiche une chaine sur la console Serie
    private void printSerialConsole(String line){
        ta_serial.append(line); 
        ta_serial.setCaretPosition(ta_serial.getDocument().getLength());
    }

    /*********************************************/
    // SOUS CLASSES
    /*********************************************/
    class taskTimeLapse extends TimerTask{
        public void run(){
            triggerShot();
            int delaySec = Integer.parseInt(tf_tl_value.getText());
            timer.schedule(new taskTimeLapse(), delaySec*1000);
        }
    }
    class taskCheck extends TimerTask{
        public void run(){
            int countdownRemain = Integer.parseInt(tf_tl_countdown.getText());
            countdownRemain--;
            if (countdownRemain < 0) {countdownRemain = Integer.parseInt(tf_tl_value.getText()) - 1;}
            tf_tl_countdown.text = countdownRemain.toString();
            timerCheck.schedule(new taskCheck(), 1000);
        }
    }

    /*********************************************/
    // SPLASH SCREEN
    /*********************************************/
    private void panelSplash(){
        // Controles du splash screen
        lbl_splash          = new JLabel();

        lbl_splash.setIcon(new ImageIcon("img/logo_px.png"));
        lbl_splash.setBounds(147,0,800,600);

        panel_splash.add(lbl_splash);
    }

    /*********************************************/
    // PANEL COMMUNICATION
    /*********************************************/
    private void panelCom(){
        // Controles de l'UI du panel com
        lbl_com             = new JLabel(lang."${languageToUse}".itemCom);
        lbl_console         = new JLabel(lang."${languageToUse}".lblConsole+":");
        lbl_serial          = new JLabel(lang."${languageToUse}".lblComSM+":");
        ta_console          = new JTextArea("");
        ta_serial           = new JTextArea("");
        btn_detect          = new JButton(lang."${languageToUse}".btnDetect);
        btn_connect         = new JButton(lang."${languageToUse}".btnConnect);

        // Titre
        lbl_com.setBounds(10,0,800,30);
        lbl_com.setFont(new Font("Verdana", Font.BOLD, 14));

        // Position des textarea
        lbl_console.setBounds(10,20,350,30);
        ta_console.setBounds(10,50,350,400);

        lbl_serial.setBounds(420,20,350,30);
        ta_serial.setBounds(420,50,350,400);

        // Config des textarea
        ta_console.setLineWrap(true);
        ta_console.setWrapStyleWord(true);
        ta_console.setFont(new Font("Consolas", Font.PLAIN, 12));
        ta_console.setForeground(Color.BLUE);
        ta_console.editable = false;

        ta_serial.setLineWrap(true);
        ta_serial.setWrapStyleWord(true);
        ta_serial.setFont(new Font("Consolas", Font.PLAIN, 12));
        ta_serial.setBackground(Color.BLACK);
        ta_serial.setForeground(Color.GREEN);
        ta_serial.editable = false;

        // Scrollbar des consoles
        sp_console = new JScrollPane(ta_console);
        sp_console.setBounds(10,50,350,400);
        sp_console.setVisible(true);
        sp_serial = new JScrollPane(ta_serial);
        sp_serial.setBounds(420,50,350,400);
        sp_serial.setVisible(true);

        // Position des boutons
        btn_detect.setBounds(175,480,250,30);
        btn_connect.setBounds(475,480,150,30);

        // Listeners d'event pour les boutons
        btn_connect.addActionListener((ActionListener) this);
        btn_detect.addActionListener((ActionListener) this);

        // Ajout des controles dans le panel
        panel_com.add(lbl_com);
        panel_com.add(lbl_console);
        panel_com.add(lbl_serial);
        panel_com.add(sp_console);
        panel_com.add(sp_serial);
        panel_com.add(btn_connect);
        panel_com.add(btn_detect);
    }        

    /*********************************************/
    // PANEL PARAM
    /*********************************************/
    private void panelParam(){
        // Controles de l'UI du panel param
        lbl_param           = new JLabel(lang."${languageToUse}".itemParam);
        lbl_param_version   = new JLabel(lang."${languageToUse}".lblFirmwareVersion+":");
        lbl_param_pmax      = new JLabel(lang."${languageToUse}".lblMaxPot+":");
        lbl_param_vmax      = new JLabel(lang."${languageToUse}".lblMaxPhoto+":");
        tf_param_version    = new JTextField(5);
        tf_param_pmax       = new JTextField(5);
        tf_param_vmax       = new JTextField(5);
        // Titre
        lbl_param.setBounds(10,0,800,30);
        lbl_param.setFont(new Font("Verdana", Font.BOLD, 14));

        lbl_param_version.setBounds(50,100,200,30);
        tf_param_version.setBounds(250,100,100,30);
        tf_param_version.editable = false;

        lbl_param_pmax.setBounds(50,150,200,30);
        tf_param_pmax.setBounds(250,150,100,30);
        tf_param_pmax.editable = false;

        lbl_param_vmax.setBounds(50,200,200,30);
        tf_param_vmax.setBounds(250,200,100,30);
        tf_param_vmax.editable = false;

        lbl_param_version.setLabelFor(tf_param_version);
        lbl_param_pmax.setLabelFor(tf_param_pmax);
        lbl_param_vmax.setLabelFor(tf_param_vmax);

        panel_param.add(lbl_param);
        panel_param.add(lbl_param_version);
        panel_param.add(lbl_param_pmax);
        panel_param.add(lbl_param_vmax);
        panel_param.add(tf_param_version);
        panel_param.add(tf_param_vmax);
        panel_param.add(tf_param_pmax);
    }

    /*********************************************/
    // PANEL PRISE DE VUE
    /*********************************************/
    private void panelPDV(){
        // Controles de l'UI du panel PDV
        lbl_pdv             = new JLabel(lang."${languageToUse}".itemPDV);
        btn_pdv             = new JButton(lang."${languageToUse}".btnPDV);

        // Titre
        lbl_pdv.setBounds(10,0,800,30);
        lbl_pdv.setFont(new Font("Verdana", Font.BOLD, 14));

        btn_pdv.setBounds(300,200,200,60);
        btn_pdv.setFont(new Font("Arial", Font.PLAIN, 20));
        btn_pdv.addActionListener((ActionListener) this);

        panel_pdv.add(lbl_pdv);
        panel_pdv.add(btn_pdv);
    }

    /*********************************************/
    // PANEL DETECTEUR ECLAIR
    /*********************************************/
    private void panelLightning(){
        // Controles de l'UI du panel Lightning
        lbl_lightning       = new JLabel(lang."${languageToUse}".itemLightning);
        lbl_sensibility     = new JLabel(lang."${languageToUse}".lblDetectorSensivity+":");
        sl_sensibility      = new JSlider();
        cb_sensibility      = new JCheckBox(lang."${languageToUse}".lblControlSensivity,false);
        rb_detectionMode0   = new JRadioButton(lang."${languageToUse}".lblVariation);
        rb_detectionMode1   = new JRadioButton(lang."${languageToUse}".lblAverage);
        rb_detectionMode2   = new JRadioButton(lang."${languageToUse}".lblForced);
        bg_detectionMode    = new ButtonGroup();

        // Titre
        lbl_lightning.setBounds(10,0,800,30);
        lbl_lightning.setFont(new Font("Verdana", Font.BOLD, 14));

        // Reglages
        lbl_sensibility.setBounds(50,30,200,30);
        cb_sensibility.setBounds(50,55,300,30);
        sl_sensibility.setBounds(50,80,700,50);
        sl_sensibility.enabled = false;
        rb_detectionMode0.setSelected(true);
        bg_detectionMode.add(rb_detectionMode0);
        bg_detectionMode.add(rb_detectionMode1);
        bg_detectionMode.add(rb_detectionMode2);
        rb_detectionMode0.setBounds(300,30,100,30);
        rb_detectionMode1.setBounds(400,30,100,30);
        rb_detectionMode2.setBounds(500,30,100,30);

        cb_sensibility.addActionListener((ActionListener) this);
        sl_sensibility.addChangeListener((ChangeListener) this);
        rb_detectionMode0.addActionListener((ActionListener) this);
        rb_detectionMode1.addActionListener((ActionListener) this);
        rb_detectionMode2.addActionListener((ActionListener) this);

        panel_lightning.add(lbl_lightning);
        panel_lightning.add(lbl_sensibility);
        panel_lightning.add(sl_sensibility);
        panel_lightning.add(cb_sensibility);
        panel_lightning.add(rb_detectionMode0);
        panel_lightning.add(rb_detectionMode1);
        panel_lightning.add(rb_detectionMode2);
    }

    /*********************************************/
    // PANEL TIMELAPSE
    /*********************************************/
    private void panelTimelapse(){
        // Controles de l'UI du panel Timelapse
        lbl_timelapse       = new JLabel(lang."${languageToUse}".itemTimelapse);
        lbl_tl_delay        = new JLabel(lang."${languageToUse}".lblTLGap+":");
        tf_tl_value         = new JTextField(5);
        tf_tl_countdown     = new JTextField(10);

        btn_tl_start        = new JButton(lang."${languageToUse}".btnStart);

        // Titre
        lbl_timelapse.setBounds(10,0,800,30);
        lbl_timelapse.setFont(new Font("Verdana", Font.BOLD, 14));
        tf_tl_countdown.setFont(new Font("Consolas", Font.BOLD, 14));
        tf_tl_countdown.enabled = false;

        // Reglages
        lbl_tl_delay.setBounds(50,100,200,25);
        tf_tl_value.setBounds(200,100,50,25);
        tf_tl_value.text = "5";

        tf_tl_countdown.setBounds(350,200,100,50);
        btn_tl_start.setBounds(350,300,100,50);

        btn_tl_start.addActionListener((ActionListener) this);
        
        panel_timelapse.add(lbl_timelapse);
        panel_timelapse.add(lbl_tl_delay);
        panel_timelapse.add(tf_tl_value);
        panel_timelapse.add(tf_tl_countdown);
        panel_timelapse.add(btn_tl_start);

    }
}
