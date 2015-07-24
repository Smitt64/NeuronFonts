using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using NeuronFontsLib;

namespace NF_CSHARP
{
    public partial class Form1 : Form
    {
        NeuronFontsClass pNf;
        public Form1()
        {
            InitializeComponent();

            pNf = new NeuronFontsClass();
            label1.Text = "Memory file: " + pNf.MemoryFile;

            pNf.openMemory();
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                pictureBox1.Load(openFileDialog1.FileName);

                int id = -1;

                pNf.recognizefile(openFileDialog1.FileName, ref id);

                if (id > -1)
                {
                    MessageBox.Show("I think its: " + Convert.ToChar(0x0410 + id));
                }
            }
        }
    }
}
