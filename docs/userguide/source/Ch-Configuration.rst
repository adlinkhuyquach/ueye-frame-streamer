.. _`Configuration`:

#############
Configuration
#############

The ADLINK Edge App uEye Frame Streamer requires an XML file to configure the app with below schema:

.. code-block:: xml

  



* ``LogLevel`` :sub:`(string)` -- Configures the logging of the app and can take one of the following values: ``TRACE``, ``DEBUG``, ``INFO``, ``WARN``, ``ERROR``, ``CRITICAL`` or ``OFF``.  The ``LogLevel`` affects how are log messages produced, with ``TRACE`` having the most and ``CRITICAL`` having the least, while ``OFF`` turns off logging completely. Defaults to "WARN" if not specified.




.. |caution|   image:: ./images/icon-caution.*
               :height: 6mm

.. |info|      image:: ./images/icon-info.*
               :height: 6mm

.. END
