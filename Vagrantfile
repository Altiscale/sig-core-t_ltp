
VAGRANTFILE_API_VERSION = "2"
Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  machines = [
    # { name: "centos7", box: "centos-7" },
    {
      name: "prometheus-6.5.14",
      box: "prometheus-6.5.14-201602120407",
      box_url: '"https://s3-us-west-1.amazonaws.com/com.altiscale.vagrant/prometheus/boxes/prometheus-6.5.14-201602120407.box?AWSAccessKeyId=AKIAIKOVEEW6FVT5JBQA&Expires=1770868554&Signature=YIhNZGY0VRD%2Bs9Z8PopwRgrvBbA%3D"'
    },
    {
      name: 'prometheus-6.6',
      box: 'prometheus-6.6.0-201512171721',
      box_url: 'https://s3-us-west-1.amazonaws.com/com.altiscale.vagrant/prometheus/boxes/prometheus-6.6.0-201512171721.box?AWSAccessKeyId=AKIAIKOVEEW6FVT5JBQA&Expires=1765959075&Signature=U0ejkPfz4V0VHbb9JBad7%2FavTro%3D'
    }
  ]

  machines.each do | machine |
    ip = machine[:ip] || '10.10.10.99'
    mem = machine[:ram] || "2048"
    cpu = machine[:cpu] || "2"
    hname = machine[:hostname] || "#{machine[:name]}.test.altiscale.com"
    box = machine[:box]
    name = machine[:name]
    box_url = machine[:box_url]

    config.vm.define name do | myvm |
      myvm.vm.provider :virtualbox do |vbox|
      vbox.customize [
        'modifyvm', :id,
        '--memory', mem,
        '--cpus', cpu,
        '--ioapic', 'on'
      ]
    end
    myvm.vm.box = box
    myvm.vm.hostname = hname
    myvm.vm.box_url = box_url
    myvm.vm.synced_folder ".", "/t_ltp", type: "rsync", rsync__exclude: ".git/"
    end
  end
end
